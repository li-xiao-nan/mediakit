//
//  ffmpeg_demuxer.cpp
//  mediacore
//
//  Created by 李楠 on 15/8/28.
//
//

#include "ffmpeg_demuxer.h"

#include "boost/bind.hpp"

#include "media/ffmpeg/ffmpeg_common.h"
#include "media/base/video_decoder_config.h"
#include "media/base/audio_decoder_config.h"

int kFFmpgeAVIOBufferSize = 1024;

namespace media {

FFmpegDemuxer::FFmpegDemuxer(TaskRunner* task_runner,
                             std::shared_ptr<net::IOChannel> data_source)
    : demux_complete_(false),
      data_source_complete(false),
      pending_seek_(false),
      duration_(0),
      task_runner_(task_runner) {
  data_source_ = data_source;
}

FFmpegDemuxer::~FFmpegDemuxer() {}

void FFmpegDemuxer::Initialize(PipelineStatusCB status_cb) {
  StartBlockingTaskRunner();
  ActionCB action_cb = boost::bind(&FFmpegDemuxer::OnOpenAVFormatContextDone,
                                   this, status_cb, _1);
  blocking_task_runner_->post(boost::bind(
      &FFmpegDemuxer::OpenAVFormatContextAction, this, status_cb, action_cb));
}

void FFmpegDemuxer::Seek(int64_t timestamp, PipelineStatusCB status_cb) {
  ActionCB action_cb =
      boost::bind(&FFmpegDemuxer::OnSeekDone, this, status_cb, _1);
  blocking_task_runner_->post(boost::bind(&FFmpegDemuxer::SeekAction, this,
                                          timestamp, status_cb, action_cb));
}

void FFmpegDemuxer::Stop() {}

int64_t FFmpegDemuxer::GetStartTime() { return start_time_; }

int64_t FFmpegDemuxer::GetDuration() { return duration_; }

int64_t FFmpegDemuxer::GetTimelineOffset() { return 0; }

DemuxerStream* FFmpegDemuxer::GetDemuxerStream(DemuxerStream::Type type) {
  for (int i = 0; i < streams_.size(); i++) {
    if (streams_[i]->type() == type) {
      return streams_[i];
    }
  }
  return NULL;
}

void FFmpegDemuxer::NotifyDemuxerCapacityAvailable() { ReadFrameIfNeeded(); }

void FFmpegDemuxer::OpenAVFormatContextAction(PipelineStatusCB status_cb,
                                              ActionCB action_cb) {
  av_register_all();

  bool result = false;
  do {
    av_io_buffer_ = (unsigned char*)av_malloc(kFFmpgeAVIOBufferSize);

    if (!av_io_buffer_) {
      result = false;
      break;
    }

    av_io_context_ =
        avio_alloc_context(av_io_buffer_, kFFmpgeAVIOBufferSize, 0, this,
                           FFmpegReadPacketCB, NULL, FFmpegSeekCB);
    if (!av_io_context_) {
      result = false;
      break;
    }
    AVInputFormat* av_input_format;
    if (av_probe_input_buffer(av_io_context_, &av_input_format, "", NULL, 0,
                              4096) != 0) {
      result = false;
      break;
    } else {
      // printf("inputFormat is :%s\n", av_input_format->long_name);
      printf("OK: success probe the input type\n");
    }
    av_io_context_->seekable = 0;
    av_format_context_ = avformat_alloc_context();
    av_format_context_->pb = av_io_context_;
    // open the input file
    if ((avformat_open_input(&av_format_context_, "", NULL, NULL)) < 0) {
      result = false;
      break;
    } else {
      printf("OK: success open the input file\n");
    }

    result = true;
    break;
  } while (1);

  task_runner_->post(boost::bind(action_cb, result));
}

void FFmpegDemuxer::OnOpenAVFormatContextDone(PipelineStatusCB status_cb,
                                              bool result) {
  if (result) {
    ActionCB action_cb =
        boost::bind(&FFmpegDemuxer::OnFindStreamInfoDone, this, status_cb, _1);
    blocking_task_runner_->post(boost::bind(
        &FFmpegDemuxer::FindStreamInfoAction, this, status_cb, action_cb));
  } else {
    status_cb(DEMUXER_OPEN_CONTEXT_FAILED);
  }
}

void FFmpegDemuxer::FindStreamInfoAction(PipelineStatusCB status_cb,
                                         ActionCB action_cb) {
  bool result = false;
  if (avformat_find_stream_info(av_format_context_, NULL) < 0) {
    printf("<lxn> %s(%d) ---> Could not find the stream information\n",
           __FILE__, __LINE__);

  } else {
    result = true;
    printf("OK: success find the streams information\n");
  }

  task_runner_->post(boost::bind(action_cb, result));
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

  start_time_ = EstimateStartTimeFromProbeAVPacketBuffer(av_format_context_);

  for (size_t i = 0; i < av_format_context_->nb_streams; i++) {
    AVStream* stream = av_format_context_->streams[i];
    AVCodecContext* av_codec_context = stream->codec;
    AVMediaType media_type = av_codec_context->codec_type;
    if (media_type == AVMEDIA_TYPE_VIDEO) {
      FFmpegDemuxerStream* video_demuxer_stream =
          static_cast<FFmpegDemuxerStream*>(
              GetDemuxerStream(DemuxerStream::VIDEO));

      if (video_demuxer_stream) continue;

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

void FFmpegDemuxer::StartBlockingTaskRunner() {
  blocking_task_runner_.reset(new boost::asio::io_service());
  blocking_io_service_work_.reset(
      new boost::asio::io_service::work(*blocking_task_runner_.get()));
  blocking_thread.reset(new boost::thread(
      boost::bind(&boost::asio::io_service::run, blocking_task_runner_.get())));
}

void FFmpegDemuxer::StopBlockingTaskRunner() {}
// blocking thread
void FFmpegDemuxer::SeekAction(int64_t timestamp, PipelineStatusCB state_cb,
                               ActionCB action_cb) {
  AVRational av_time_base = {1, AV_TIME_BASE};
  int ret = av_seek_frame(av_format_context_, -1,
                          ConvertToTimeBase(timestamp, av_time_base),
                          AVSEEK_FLAG_BACKWARD);
  task_runner_->post(boost::bind(action_cb, (ret >= 0 ? true : false)));
}

void FFmpegDemuxer::OnSeekDone(PipelineStatusCB status_cb, bool result) {
  if (result) {
    status_cb(PIPELINE_OK);
  } else {
    status_cb(PIPELINE_ERROR_SEEK_FAILED);
    return;
  }

  ReadFrameIfNeeded();
}

void FFmpegDemuxer::ReadFrameIfNeeded() {
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
  blocking_task_runner_->post(task);
}

void FFmpegDemuxer::ReadFrameAction(
    std::shared_ptr<EncodedAVFrame> encoded_avframe, ActionCB action_cb) {
  bool result = !av_read_frame(av_format_context_, encoded_avframe.get());
  if (!result) {
    std::cout << "av_read_frame failed" << std::endl;
  }

  task_runner_->post(boost::bind(action_cb, result));
}

void FFmpegDemuxer::OnReadFrameDone(
    std::shared_ptr<EncodedAVFrame> encoded_avframe, bool result) {
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
    } else {
      std::cout << __FILE__ << "<" << __LINE__ << ">" << std::endl;
    }
  }
  ReadFrameIfNeeded();
}

void FFmpegDemuxer::ReadPacketCB(unsigned char* buffer, int buffer_size) {
  data_source_->read(buffer, buffer_size);
}

int64_t FFmpegDemuxer::SeekCB(int64_t offset, int whence) {
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
  return data_source_->tell();
}
int FFmpegDemuxer::FFmpegReadPacketCB(void* opaque, unsigned char* buffer,
                                      int buffer_size) {
  FFmpegDemuxer* ffmpeg_demuxer = static_cast<FFmpegDemuxer*>(opaque);
  ffmpeg_demuxer->ReadPacketCB(buffer, buffer_size);
}
int64_t FFmpegDemuxer::FFmpegSeekCB(void* opaque, int64_t offset, int whence) {
  FFmpegDemuxer* ffmpeg_demuxer = static_cast<FFmpegDemuxer*>(opaque);
  ffmpeg_demuxer->SeekCB(offset, whence);
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
  for (int i = 0; i < streams_.size(); i++) {
    streams_[i]->ShowConfigInfo();
  }
}
}  // namespace media
