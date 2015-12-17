#ifndef MEDIA_DECODER_AUDIO_FRAME_STREAM_H
#define MEDIA_DECODER_AUDIO_FRAME_STREAM_H

#include <vector>
#include <queue>
#include <memory>
#include "boost/thread/thread.hpp"

#include "base/base_type.h"
#include "media/base/audio_frame.h"
#include "media/decoder/audio_decoder.h"
#include "media/decoder/video_decoder.h"

const int kMaxAudioFrameQueueSize = 4;

namespace media {

class DemuxerStream;

class AudioFrameStream {
 public:
  enum Status {
    STATUS_OK,
    STATUS_DEMUXER_READ_ERROR,
    STATUS_DECODE_ERROR,
    STATUS_DECODE_ABORT,
    STATUS_DECODE_COMPLETED,
  };

  typedef std::vector<AudioDecoder*> VecAudioDecoders;
  typedef boost::function<void(bool successed)> InitCB;
  typedef boost::function<void(Status status, std::shared_ptr<AudioFrame>)>
      ReadCB;

  AudioFrameStream(TaskRunner* task_runner, const VecAudioDecoders& decoders);
  ~AudioFrameStream();
  void Initialize(DemuxerStream* stream, InitCB init_cb);
  void Read(ReadCB read_cb);

 private:
  enum State {
    STATE_UNINITIALIZED,
    STATE_INITIALIZING,
    STATE_DECODE_COMPLETED,
    STATE_NORMAL,
    STATE_ERROR
  };

  void DecoderOutputCB(std::shared_ptr<AudioFrame> audio_frame);
  void OnDecoderInitDone(bool result);
  void OnDecodeDone(AudioDecoder::Status status);
  void DecodeFrameIfNeeded();
  bool CanDecodeMore();
  void ReadFromDemuxerStream();
  void OnReadFromDemuxerStreamDone(
      std::shared_ptr<EncodedAVFrame> encoded_av_frame);
  std::shared_ptr<AudioFrame> ReadReadyFrameLocked();
  std::shared_ptr<AudioFrame> ReadReadyFrameNoLocked();
  void EnqueueFrameLocked(std::shared_ptr<AudioFrame> audio_frame);

  ReadCB read_cb_;
  InitCB init_cb_;
  State state_;
  bool is_demux_completed_;
  bool is_decode_completed_;
  boost::mutex frame_queue_mutex_;
  VecAudioDecoders vec_decoders_;
  AudioDecoder* decoder_;
  DemuxerStream* demuxer_stream_;
  TaskRunner* task_runner_;
  std::deque<std::shared_ptr<AudioFrame> > frame_queue_;
};

}  // namespace media

#endif