//
//  ffmpeg_demuxer.cpp
//  mediacore
//
//  Created by 李楠 on 15/8/28.
//
//

#include "ffmpeg_demuxer.h"

#include "libavutil/log.h"
#include "boost/bind.hpp"

#include "media/ffmpeg/ffmpeg_common.h"
#include "media/base/video_decoder_config.h"
#include "media/base/audio_decoder_config.h"
#include "base/message_loop_thread_manager.h"
#include "log/log_wrapper.h"

int kFFmpgeAVIOBufferSize = 1024*3;

namespace media {

FFmpegDemuxer::FFmpegDemuxer(std::shared_ptr<net::IOChannel> data_source)
    : demux_complete_(false),
      data_source_complete(false),
      pending_seek_(false),
      duration_(0),
      pause_state_(false),
      demuxer_delegate_(nullptr){
  data_source_ = data_source;
}

FFmpegDemuxer::~FFmpegDemuxer() {
  avformat_close_input(&av_format_context_);
  if(av_format_context_) {
    avformat_free_context(av_format_context_);
    av_format_context_ = nullptr;
  }
  if(av_io_context_) {
    av_free(av_io_context_);
    av_io_context_ = nullptr;
  }
}

void FFmpegDemuxer::Initialize(PipelineStatusCB status_cb) {
  // av_log_set_callback(ffmpeg_log_callback);
  // av_log_set_level(LOG_LEVEL_ERROR);
  
  ActionCB action_cb = boost::bind(&FFmpegDemuxer::OnOpenAVFormatContextDone,
                                   this, status_cb, _1);
  PostTask(TID_DEMUXER,(boost::bind(
      &FFmpegDemuxer::OpenAVFormatContextAction, this, status_cb, action_cb)));
}

void FFmpegDemuxer::Seek(int64_t timestamp, PipelineStatusCB status_cb) {
  ActionCB action_cb =
      boost::bind(&FFmpegDemuxer::OnSeekDone, this, status_cb, _1);
  LOGGING(LOG_LEVEL_DEBUG) << "Post SeekAction Task";
  PostTask(TID_DEMUXER, (boost::bind(&FFmpegDemuxer::SeekAction, this,
                                          timestamp, status_cb, action_cb)));
  return;
}

void FFmpegDemuxer::Stop() {}

int64_t FFmpegDemuxer::GetStartTime() { return start_time_; }

int64_t FFmpegDemuxer::GetDuration() { return duration_; }

int64_t FFmpegDemuxer::GetTimelineOffset() { return 0; }

DemuxerStream* FFmpegDemuxer::GetDemuxerStream(DemuxerStream::Type type) {
  for (size_t i = 0; i < streams_.size(); i++) {
    if (streams_[i]->type() == type) {
      return streams_[i];
    }
  }
  return NULL;
}

// decode thread
void FFmpegDemuxer::NotifyDemuxerCapacityAvailable() { 
  ReadFrameIfNeeded(); 
}

void FFmpegDemuxer::OpenAVFormatContextAction(PipelineStatusCB status_cb,
                                              ActionCB action_cb) {
  AUTORUNTIMER("FFmpegDemuxer::OpenAVFormatContextAction");
  av_register_all();

  bool result = false;
    av_format_context_ = avformat_alloc_context();
    av_io_buffer_ = (unsigned char*)av_malloc(kFFmpgeAVIOBufferSize);
    av_io_context_ =
        avio_alloc_context(av_io_buffer_, kFFmpgeAVIOBufferSize, 0, this,
                           FFmpegReadPacketCB, NULL, FFmpegSeekCB);

    av_io_context_->seekable = 0;
    av_io_context_->write_flag = false;
    // Enable fast, but inaccurate seeks for MP3.
    av_format_context_->flags; //|= AVFMT_FLAG_FAST_SEEK;
    av_format_context_->pb = av_io_context_;
    // open the input file
    int error_code = avformat_open_input(&av_format_context_, NULL, NULL, NULL);
    if (error_code != 0) {
      result = false;
      LOGGING(LOG_LEVEL_ERROR) << "open video file failed";
      if(demuxer_delegate_) {
        demuxer_delegate_->OnOpenMediaFileFailed(
          data_source_->GetFileName(), error_code, "failed");
      }
      return;
    } else {
      LOGGING(LOG_LEVEL_INFO) << "open video file success";
      if(demuxer_delegate_) {
        AVRational av_format_context_time_base = {1, AV_TIME_BASE};
        int64_t max_duration = std::max(max_duration, av_format_context_->duration);
        int64_t duration_by_second = max_duration / AV_TIME_BASE;
        MediaInfo media_info;
        media_info.video_duration_ = duration_by_second * 1000;
        demuxer_delegate_->OnGetMediaInfo(media_info);
      }
    }
    result = true;
    PostTask(TID_DECODE, boost::bind(action_cb, result));
}

void FFmpegDemuxer::OnOpenAVFormatContextDone(PipelineStatusCB status_cb,
                                              bool result) {
  TRACEPOINT;
  if (result) {
    ActionCB action_cb =
        boost::bind(&FFmpegDemuxer::OnFindStreamInfoDone, this, status_cb, _1);
    PostTask(TID_DEMUXER, (boost::bind(
        &FFmpegDemuxer::FindStreamInfoAction, this, status_cb, action_cb)));
    TRACEPOINT;
  } else {
    status_cb(DEMUXER_OPEN_CONTEXT_FAILED);
  }
  TRACEPOINT;
}

void FFmpegDemuxer::FindStreamInfoAction(PipelineStatusCB status_cb,
                                         ActionCB action_cb) {
  TRACEPOINT;
  AUTORUNTIMER("FFmpegDemuxer::FindStreamInfoAction");
  bool result = false;
  if (avformat_find_stream_info(av_format_context_, NULL) < 0) {
    LOGGING(LOG_LEVEL_ERROR)<< "Could not find the stream information";
  } else {
    result = true;
    LOGGING(LOG_LEVEL_ERROR) << "OK: success find the streams information";
  }

  PostTask(TID_DECODE, boost::bind(action_cb, result));
}

void FFmpegDemuxer::OnFindStreamInfoDone(PipelineStatusCB status_cb,
                                         bool result) {
  if (!result) {
    status_cb(DEMUXER_FIND_STREAM_INFO_FAILED);
    return;
  }

  int64_t max_duration = 0;
  int64_t min_start_time = 0;

  AVRational av_format_context_time_base = {1, AV_TIME_BASE};
  max_duration = std::max(max_duration, av_format_context_->duration);
  int64_t duration_by_second = max_duration / AV_TIME_BASE;
  LOGGING(LOG_LEVEL_INFO) << "VideoDuration:" << (duration_by_second/(60*60))
    << ":" << (duration_by_second/60%60)
    << ":" << duration_by_second%60;

  start_time_ = EstimateStartTimeFromProbeAVPacketBuffer(av_format_context_);
  LOGGING(LOG_LEVEL_INFO) << "StreamCount:" << av_format_context_->nb_streams;
  for (size_t i = 0; i < av_format_context_->nb_streams; i++) {
    AVStream* stream = av_format_context_->streams[i];
    AVCodecContext* av_codec_context = stream->codec;
    AVMediaType media_type = av_codec_context->codec_type;
    if (media_type == AVMEDIA_TYPE_VIDEO) {
      FFmpegDemuxerStream* video_demuxer_stream =
          static_cast<FFmpegDemuxerStream*>(
              GetDemuxerStream(DemuxerStream::VIDEO));

      if (video_demuxer_stream) continue;
      video_stream_index_ = i;
      SetVideoStreamTimeBase(stream->time_base);
      video_demuxer_stream = new FFmpegDemuxerStream(this, stream);
      VideoDecoderConfig video_decoder_config;
      AVStreamToVideoDecoderConfig(stream, &video_decoder_config);
      video_demuxer_stream->set_video_decoder_config(video_decoder_config);
      streams_.push_back(video_demuxer_stream);
      max_duration = std::max(max_duration, video_demuxer_stream->duration());
    } else if (media_type == AVMEDIA_TYPE_AUDIO) {
      FFmpegDemuxerStream* audio_demuxer_stream =
          static_cast<FFmpegDemuxerStream*>(
              GetDemuxerStream(DemuxerStream::AUDIO));
      if (audio_demuxer_stream) continue;
      audio_stream_index_ = i;
      SetAudioStreamTimeBase(stream->time_base);
      audio_demuxer_stream = new FFmpegDemuxerStream(this, stream);
      AudioDecoderConfig audio_decoder_config;
      AVStreamToAudioDecoderConfig(stream, &audio_decoder_config);
      audio_demuxer_stream->set_audio_decoder_config(audio_decoder_config);
      streams_.push_back(audio_demuxer_stream);
      max_duration = std::max(max_duration, audio_demuxer_stream->duration());
    }
  }

  duration_ = max_duration;
  status_cb(PIPELINE_OK);
}

void FFmpegDemuxer::SetDelegate(DemuxerDelegate* delegate) {
  demuxer_delegate_ = delegate;
}

void FFmpegDemuxer::SeekAction(int64_t timestamp_ms, PipelineStatusCB state_cb,
                               ActionCB action_cb) {
  LOGGING(LOG_LEVEL_DEBUG)<<"Enter";
  ScopeTimeCount("SeekAction");
  int ret = 
    av_seek_frame(av_format_context_, -1, timestamp_ms * 1000, AVSEEK_FLAG_BACKWARD);
  for(auto key : streams_) {
      key->ClearEncodedAVFrameBuffer();
  }
  avformat_flush(av_format_context_);
  avio_flush(av_format_context_->pb);
  Resume();
  //AfterSeekReadFirstPacket();
  ReadFrameIfNeeded();
  //DropBufferedDataTest(timestamp_ms);
  PostTask(TID_DECODE, boost::bind(action_cb, (ret >= 0 ? true : false)));
}

std::string FFmpegDemuxer::GetVideoUrl() {
  return data_source_->GetFileName();
}

void FFmpegDemuxer::AfterSeekReadFirstPacket() {
  std::shared_ptr<EncodedAVFrame> encoded_avframe(new AVPacket());
  bool result = !av_read_frame(av_format_context_, encoded_avframe.get());
  if (!result) {
    LOGGING(LOG_LEVEL_ERROR) << "av_read_frame failed";
    return;
  }
  int64_t seek_result_pts;
  if(encoded_avframe->stream_index == 0)
    seek_result_pts = TimeBaseToMillionSecond(encoded_avframe->pts, GetAudioStreamTimeBase());
  else 
    seek_result_pts = TimeBaseToMillionSecond(encoded_avframe->pts, GetVideoStreamTimeBase());
  if(demuxer_delegate_) {
    demuxer_delegate_->OnUpdateAlignedSeekTimestamp(seek_result_pts);
  }
  PostTask(TID_DECODE, 
    boost::bind(&FFmpegDemuxer::OnReadFrameDone, this, encoded_avframe, true));
}

void FFmpegDemuxer::OnSeekDone(PipelineStatusCB status_cb, bool result) {
  if (result) {
    status_cb(PIPELINE_OK);
  } else {
    status_cb(PIPELINE_ERROR_SEEK_FAILED);
    return;
  }
}

// decode thread
void FFmpegDemuxer::ReadFrameIfNeeded() {
  if(pause_state_) {
    LOGGING(LOG_LEVEL_INFO) << "FFmpegDemuxer::ReadFrameIfNeeded, paused";
    return;
  }
  FFmpegDemuxerStream* video_stream =
      static_cast<FFmpegDemuxerStream*>(GetDemuxerStream(DemuxerStream::VIDEO));
  FFmpegDemuxerStream* audio_stream =
      static_cast<FFmpegDemuxerStream*>(GetDemuxerStream(DemuxerStream::AUDIO));

  if (!audio_stream->HasAvailableCapacity() &&
      !video_stream->HasAvailableCapacity()) {
    return;
  }
  std::shared_ptr<EncodedAVFrame> encoded_avframe(new AVPacket());
  ActionCB action_cb =
      boost::bind(&FFmpegDemuxer::OnReadFrameDone, this, encoded_avframe, _1);
  AsyncTask task = boost::bind(&FFmpegDemuxer::ReadFrameAction, this,
                               encoded_avframe, action_cb);
  PostTask(TID_DEMUXER, task);
}

// demux thread
void FFmpegDemuxer::ReadFrameAction(
    std::shared_ptr<EncodedAVFrame> encoded_avframe, ActionCB action_cb) {
  bool result = !av_read_frame(av_format_context_, encoded_avframe.get());
  if (!result) {
    LOGGING(LOG_LEVEL_ERROR) <<  "av_read_frame failed";
  }
  PostTask(TID_DECODE, boost::bind(action_cb, result));
}

// decode thread
void FFmpegDemuxer::OnReadFrameDone(
    std::shared_ptr<EncodedAVFrame> encoded_avframe, bool result) {
  if (pause_state_) {
    LOGGING(LOG_LEVEL_DEBUG) << "PAUSING ...";
    return;
  }
  FFmpegDemuxerStream* video_stream =
      static_cast<FFmpegDemuxerStream*>(GetDemuxerStream(DemuxerStream::VIDEO));
  FFmpegDemuxerStream* audio_stream =
      static_cast<FFmpegDemuxerStream*>(GetDemuxerStream(DemuxerStream::AUDIO));

  if (result) {
    int encoded_avframe_stream_id = encoded_avframe->stream_index;
    if (audio_stream &&
        encoded_avframe_stream_id == audio_stream->stream_index()) {
      audio_stream->EnqueueEncodedFrame(encoded_avframe);
    } else if (video_stream &&
               encoded_avframe_stream_id == video_stream->stream_index()) {
      video_stream->EnqueueEncodedFrame(encoded_avframe);
      TraceAVPacketProcess(encoded_avframe->pts);
    } else {
    }
  }
  ReadFrameIfNeeded();
}

int FFmpegDemuxer::ReadPacketCB(unsigned char* buffer, int buffer_size) {
 return data_source_->read(buffer, buffer_size);
}

long FFmpegDemuxer::SeekCB(long offset, int whence) {
  if (whence == SEEK_SET) {
    if (offset < 0)
      return -1;
    else
      data_source_->seek(offset);
  } else if (whence == SEEK_CUR) {
    data_source_->seek(data_source_->tell() +
                       static_cast<std::streamoff>(offset));
  } else if (whence == SEEK_END) {
    data_source_->seek(1024);
  } else {
    return 0;
  }
  long result = data_source_->tell();
  return result;
}
int FFmpegDemuxer::FFmpegReadPacketCB(void* opaque, unsigned char* buffer,
                                      int buffer_size) {
  FFmpegDemuxer* ffmpeg_demuxer = static_cast<FFmpegDemuxer*>(opaque);
  return ffmpeg_demuxer->ReadPacketCB(buffer, buffer_size);
}
int64_t FFmpegDemuxer::FFmpegSeekCB(void* opaque, int64_t offset, int whence) {
  FFmpegDemuxer* ffmpeg_demuxer = static_cast<FFmpegDemuxer*>(opaque);
  ffmpeg_demuxer->SeekCB(static_cast<long>(offset), whence);
  return 0;
}

int64_t FFmpegDemuxer::EstimateStartTimeFromProbeAVPacketBuffer(
    AVFormatContext* av_format_context) {
#if 0
  if(!av_format_context->internal->packet_buffer)
    return 0;

  int64_t min_start_time = 0;

  AVPacketList* packet_buffer_list = av_format_context->internal->packet_buffer;
  AVPacketList* packet_buffer_list_end = av_format_context->internal->packet_buffer_end;
  AVPacket* av_packet_iterator;
  while(packet_buffer_list != packet_buffer_list_end){
    av_packet_iterator = &packet_buffer_list->pkt;
    int pkt_stream_index = av_packet_iterator->stream_index;
    AVRational time_base = av_format_context_->streams[pkt_stream_index]->time_base;
    int start_time = ConvertToTimeBase(av_packet_iterator->pts, time_base);
    start_time = (min_start_time == 0) ? std::max(min_start_time, start_time) : std::min(min_start_time, start_time);
    
    packet_buffer_list = packet_buffer_list->next;
  }
  return min_start_time;
#endif
  return av_format_context->start_time;
}

void FFmpegDemuxer::ShowMediaConfigInfo() {
  for (size_t i = 0; i < streams_.size(); i++) {
    streams_[i]->ShowConfigInfo();
  }
}

void FFmpegDemuxer::Pause() {
  pause_state_ = true;
}

void FFmpegDemuxer::Resume() {
  pause_state_ = false;
}
}  // namespace media
