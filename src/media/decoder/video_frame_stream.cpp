#include "video_frame_stream.h"

#include "boost/bind.hpp"
#include "video_decoder.h"
#include "media/demuxer/demuxer_stream.h"

namespace media {
VideoFrameStream::VideoFrameStream(const VecVideoDecoders& vec_video_decoders){
  for (size_t i = 0; i < vec_video_decoders.size(); i++) {
    vec_video_decoders_.push_back(vec_video_decoders[i]);
  }
}

VideoFrameStream::~VideoFrameStream() {
}

void VideoFrameStream::Initialize(DemuxerStream* stream, InitCB init_cb) {
  init_cb_ = init_cb;
  demuxer_stream_ = stream;
  decoder_ = vec_video_decoders_[0];
  state_ = STATE_INITIALIZING;
  assert(decoder_ != NULL);

  decoder_->Initialize(
      demuxer_stream_->video_decoder_config(),
      boost::bind(&VideoFrameStream::OnVideoDecoderInitDone, this, _1),
      boost::bind(&VideoFrameStream::DecoderOutputCB, this, _1));
}

// decode thread
void VideoFrameStream::Read(ReadCB read_cb) {
  std::shared_ptr<VideoFrame> video_frame = ReadReadyVideoFrameLocked();
  if (video_frame.get()) {
    read_cb(STATUS_OK, video_frame);
  } else {
    read_cb_ = read_cb;
  }
  DecodeFrameIfNeeded();
}

void VideoFrameStream::OnVideoDecoderInitDone(bool result) {
  if (result) {
    state_ = STATE_NORMAL;
  } else {
    state_ = STATE_ERROR;
  }

  init_cb_(result);
}

void VideoFrameStream::OnDecodeDone(VideoDecoder::Status status) {
  Status video_frame_stream_status;
  switch (status) {
    case VideoDecoder::STATUS_OK:
      video_frame_stream_status = STATUS_OK;
      break;
    case VideoDecoder::STATUS_DECODE_ABORT:
      video_frame_stream_status = STATUS_DECODE_ABORT;
      break;
    case VideoDecoder::STATUS_DECODE_ERROR:
      state_ = STATE_ERROR;
      video_frame_stream_status = STATUS_DECODE_ERROR;
      break;
    case VideoDecoder::STATUS_DECODE_COMPLETED:
      state_ = STATE_DECODE_COMPLETED;
      video_frame_stream_status = STATUS_DECODE_COMPLETED;
      break;
    default:
      break;
  }

  if (read_cb_) {
    read_cb_(video_frame_stream_status, ReadReadyVideoFrameNoLocked());
    read_cb_ = NULL;
  }
  DecodeFrameIfNeeded();
}

std::shared_ptr<VideoFrame> VideoFrameStream::ReadReadyVideoFrameNoLocked() {
  std::shared_ptr<VideoFrame> video_frame;
  if (!video_frame_queue_.empty()) {
    video_frame = video_frame_queue_.front();
    video_frame_queue_.pop_front();
  }
  return video_frame;
}

// decode thread
std::shared_ptr<VideoFrame> VideoFrameStream::ReadReadyVideoFrameLocked() {
  boost::mutex::scoped_lock lock(video_frame_queue_mutex);
  std::shared_ptr<VideoFrame> video_frame;
  if (!video_frame_queue_.empty()) {
    video_frame = video_frame_queue_.front();
    video_frame_queue_.pop_front();
  }
  return video_frame;
}

// decode thread
void VideoFrameStream::ReadFromDemuxerStream() {
  demuxer_stream_->Read(
      boost::bind(&VideoFrameStream::OnReadFromDemuxerStreamDone, this, _1));
}

void VideoFrameStream::OnReadFromDemuxerStreamDone(
    std::shared_ptr<EncodedAVFrame> encoded_av_frame) {
  if (encoded_av_frame.get() ||
      (!encoded_av_frame.get() && demuxer_stream_->is_demux_complete())) {
    decoder_->Decode(encoded_av_frame,
                     boost::bind(&VideoFrameStream::OnDecodeDone, this, _1));
  } else {
    // do nothing
    assert(0);
  }
}

void VideoFrameStream::DecoderOutputCB(
    std::shared_ptr<VideoFrame> video_frame) {
  video_frame_queue_.push_back(video_frame);
}

// decode thread
void VideoFrameStream::DecodeFrameIfNeeded() {
  if (!CanDecodeMore())
    return;
  ReadFromDemuxerStream();
}

bool VideoFrameStream::CanDecodeMore() {
  return (video_frame_queue_.size() < kMaxVideoFrameQueueSize) &&
         (state_ == STATE_NORMAL);
}

void VideoFrameStream::ClearBuffer() {
  std::deque<std::shared_ptr<VideoFrame> > empty;
  std::swap(empty, video_frame_queue_);
  demuxer_stream_->ClearEncodedAVFrameBuffer();
}

void VideoFrameStream::ShowStateInfo() {
  LOGGING(LOG_LEVEL_DEBUG)<<"video_frame_queue_"<< video_frame_queue_.size();
  demuxer_stream_->ShowStateInfo();
}

void VideoFrameStream::ShowState() {
  LOGGING(LOG_LEVEL_DEBUG) << "video_frame_queue_:" << video_frame_queue_.size();
}

}  // namespace media