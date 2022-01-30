#include "media/decoder/audio_frame_stream.h"
#include "media/demuxer/demuxer_stream.h"
#include "base/message_loop_thread_manager.h"

namespace media {
AudioFrameStream::AudioFrameStream(const VecAudioDecoders& decoders) {
  for (size_t i = 0; i < decoders.size(); i++) {
    vec_decoders_.push_back(decoders[i]);
  }
}

AudioFrameStream::~AudioFrameStream() {
}

void AudioFrameStream::Initialize(DemuxerStream* stream, InitCB init_cb) {
  init_cb_ = init_cb;
  demuxer_stream_ = stream;
  decoder_ = vec_decoders_[0];
  state_ = STATE_INITIALIZING;
  assert(decoder_ != NULL);

  decoder_->Initialize(
      demuxer_stream_->audio_decoder_config(),
      boost::bind(&AudioFrameStream::OnDecoderInitDone, this, _1),
      boost::bind(&AudioFrameStream::DecoderOutputCB, this, _1));
}

// audio sink thread
void AudioFrameStream::Read(ReadCB read_cb) {
  for (;;) {
    std::shared_ptr<AudioFrame> audio_frame = ReadReadyFrameLocked();
    if (audio_frame.get()) {
      read_cb(STATUS_OK, audio_frame);
    } else {
      break;
    }
  }

  read_cb_ = read_cb;
  PostTask(TID_DECODE, boost::bind(&AudioFrameStream::DecodeFrameIfNeeded, this));
}

void AudioFrameStream::DecoderOutputCB(
    std::shared_ptr<AudioFrame> audio_frame) {
  EnqueueFrameLocked(audio_frame);
}

void AudioFrameStream::EnqueueFrameLocked(
    std::shared_ptr<AudioFrame> audio_frame) {
  frame_queue_.push_back(audio_frame);
}

void AudioFrameStream::OnDecoderInitDone(bool result) {
  if (result) {
    state_ = STATE_NORMAL;
  } else {
    state_ = STATE_ERROR;
  }

  init_cb_(result);
}

void AudioFrameStream::OnDecodeDone(AudioDecoder::Status status) {
  Status audio_frame_stream_status;
  switch (status) {
    case AudioDecoder::STATUS_OK:
      audio_frame_stream_status = STATUS_OK;
      break;
    case AudioDecoder::STATUS_DECODE_ABORT:
      audio_frame_stream_status = STATUS_DECODE_ABORT;
      break;
    case AudioDecoder::STATUS_DECODE_ERROR:
      state_ = STATE_ERROR;
      audio_frame_stream_status = STATUS_DECODE_ERROR;
      break;
    case AudioDecoder::STATUS_DECODE_COMPLETED:
      state_ = STATE_DECODE_COMPLETED;
      audio_frame_stream_status = STATUS_DECODE_COMPLETED;
      break;
    default:
      break;
  }

  if (read_cb_) {
    read_cb_(audio_frame_stream_status, ReadReadyFrameNoLocked());
    read_cb_ = NULL;
  }
  DecodeFrameIfNeeded();
}

void AudioFrameStream::DecodeFrameIfNeeded() {
  if (!CanDecodeMore())
    return;
  ReadFromDemuxerStream();
}

bool AudioFrameStream::CanDecodeMore() {
  return (frame_queue_.size() < kMaxAudioFrameQueueSize) &&
         (state_ == STATE_NORMAL);
}

void AudioFrameStream::ReadFromDemuxerStream() {
  demuxer_stream_->Read(
      boost::bind(&AudioFrameStream::OnReadFromDemuxerStreamDone, this, _1));
}

void AudioFrameStream::OnReadFromDemuxerStreamDone(
    std::shared_ptr<EncodedAVFrame> encoded_av_frame) {
  if (encoded_av_frame.get() ||
      (!encoded_av_frame.get() && demuxer_stream_->is_demux_complete())) {
    decoder_->Decode(encoded_av_frame,
                     boost::bind(&AudioFrameStream::OnDecodeDone, this, _1));
  } else {
    // do nothing
    assert(0);
  }
}
std::shared_ptr<AudioFrame> AudioFrameStream::ReadReadyFrameLocked() {
  boost::mutex::scoped_lock lock(frame_queue_mutex_);
  std::shared_ptr<AudioFrame> audio_frame;
  if (!frame_queue_.empty()) {
    audio_frame = frame_queue_.front();
    frame_queue_.pop_front();
  }
  return audio_frame;
}
std::shared_ptr<AudioFrame> AudioFrameStream::ReadReadyFrameNoLocked() {
  boost::mutex::scoped_lock lock(frame_queue_mutex_);
  std::shared_ptr<AudioFrame> audio_frame;
  if (!frame_queue_.empty()) {
    audio_frame = frame_queue_.front();
    frame_queue_.pop_front();
  }
  return audio_frame;
}

void AudioFrameStream::ClearBuffer() {
  std::deque<std::shared_ptr<AudioFrame> > empty;
  std::swap(empty, frame_queue_);
}

}  // namespace media