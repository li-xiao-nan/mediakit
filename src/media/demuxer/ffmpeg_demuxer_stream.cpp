//
//  ffmpeg_demuxer_stream.cpp
//  mediacore
//
//  Created by 李楠 on 15/8/31.
//
//

#include "ffmpeg_demuxer_stream.h"
#include "media/demuxer/ffmpeg_demuxer.h"
#include "media/FFmpeg/ffmpeg_common.h"
const int kMaxQueueSize = 2<<6;

namespace media {
FFmpegDemuxerStream::FFmpegDemuxerStream(FFmpegDemuxer* demuxer,
                                         AVStream* stream)
    : is_demux_complete_(false), demuxer_(demuxer) {
  stream_index_ = stream->index;
  time_base_ = stream->time_base;
  duration_ = ConvertToTimeBase(stream->duration, stream->time_base);
  switch (stream->codec->codec_type) {
    case AVMEDIA_TYPE_VIDEO:
      type_ = VIDEO;
      break;
    case AVMEDIA_TYPE_AUDIO:
      type_ = AUDIO;
      break;
    default:
      type_ = NONE;
      break;
  }
}

FFmpegDemuxerStream::~FFmpegDemuxerStream() {
}

DemuxerStream::Type FFmpegDemuxerStream::type() {
  return type_;
}

void FFmpegDemuxerStream::EnqueueEncodedFrame(
    std::shared_ptr<EncodedAVFrame> frame) {
  encoded_avframe_queue_.push(frame);
  switch (type_) {
  case AUDIO:
	  //std::cout << "[AUDIO] queue is empty" << std::endl;
	  break;
  case VIDEO:
	  //std::cout << "[VIDEO] queue size:" << encoded_avframe_queue_.size()<<std::endl;
	  break;
  }
  if (read_cb_) {
    std::shared_ptr<EncodedAVFrame> av_frame = encoded_avframe_queue_.front();
    encoded_avframe_queue_.pop();
    read_cb_(av_frame);
    read_cb_ = NULL;
  }
}

void FFmpegDemuxerStream::Read(ReadCB read_cb) {
  if (encoded_avframe_queue_.size() < kMaxQueueSize) {
    demuxer_->NotifyDemuxerCapacityAvailable();
  }
  std::shared_ptr<EncodedAVFrame> encoded_avframe;
  if (encoded_avframe_queue_.empty()) {
	  switch (type_) {
	  case AUDIO:
		  //std::cout << "[AUDIO] queue is empty" << std::endl;
		  break;
	  case VIDEO:
		  //std::cout << "[VIDEO] queue is empty" << std::endl;
		  break;
	  }
    read_cb_ = read_cb;
  } else {
    encoded_avframe = encoded_avframe_queue_.front();
    encoded_avframe_queue_.pop();
    read_cb(encoded_avframe);
  }
}

void FFmpegDemuxerStream::set_demux_complete() {
  is_demux_complete_ = true;
}

bool FFmpegDemuxerStream::is_demux_complete() {
  return is_demux_complete_;
}

const VideoDecoderConfig& FFmpegDemuxerStream::video_decoder_config() const {
  return video_decoder_config_;
}

void FFmpegDemuxerStream::set_video_decoder_config(
    const VideoDecoderConfig& video_decoder_config) {
  video_decoder_config_ = video_decoder_config;
}

const AudioDecoderConfig& FFmpegDemuxerStream::audio_decoder_config() const {
  return audio_decoder_config_;
}

void FFmpegDemuxerStream::set_audio_decoder_config(
    const AudioDecoderConfig& audio_decoder_config) {
  audio_decoder_config_ = audio_decoder_config;
}

bool FFmpegDemuxerStream::HasAvailableCapacity() {
  return encoded_avframe_queue_.size() < kMaxQueueSize;
}

void FFmpegDemuxerStream::ShowConfigInfo() {
  switch (type_) {
    case AUDIO:
      audio_decoder_config_.ShowAudioConfigInfo();
      break;
    case VIDEO:
      video_decoder_config_.ShowVideoConfigInfo();
      break;
  }
}
}  // namespace media
