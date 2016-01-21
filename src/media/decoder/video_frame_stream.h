#ifndef MEDIA_DECODER_VIDEO_FRAME_STREAM_H
#define MEDIA_DECODER_VIDEO_FRAME_STREAM_H

#include <vector>
#include <queue>
#include <memory>
#include "boost/thread/thread.hpp"

#include "base/base_type.h"
#include "media/base/video_frame.h"
#include "media/decoder/video_decoder.h"

const int kMaxVideoFrameQueueSize = 2<<5;

namespace media {

class VideoDecoder;
class DemuxerStream;

class VideoFrameStream {
 public:
  enum Status {
    STATUS_OK,
    STATUS_DEMUXER_READ_ERROR,
    STATUS_DECODE_ERROR,
    STATUS_DECODE_ABORT,
    STATUS_DECODE_COMPLETED,
  };

  typedef std::vector<VideoDecoder*> VecVideoDecoders;
  typedef boost::function<void(bool successed)> InitCB;
  typedef boost::function<void(Status status, std::shared_ptr<VideoFrame>)>
      ReadCB;

  VideoFrameStream(TaskRunner* task_runner, const VecVideoDecoders& decoders);
  ~VideoFrameStream();
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

  void DecoderOutputCB(std::shared_ptr<VideoFrame> video_frame);
  void OnVideoDecoderInitDone(bool result);
  void OnDecodeDone(VideoDecoder::Status status);
  void DecodeFrameIfNeeded();
  bool CanDecodeMore();
  void ReadFromDemuxerStream();
  void OnReadFromDemuxerStreamDone(
      std::shared_ptr<EncodedAVFrame> encoded_av_frame);
  std::shared_ptr<VideoFrame> ReadReadyVideoFrameLocked();
  std::shared_ptr<VideoFrame> ReadReadyVideoFrameNoLocked();

  ReadCB read_cb_;
  InitCB init_cb_;
  State state_;
  bool is_demux_completed;
  bool is_decode_completed;
  boost::mutex video_frame_queue_mutex;
  VecVideoDecoders vec_video_decoders_;
  VideoDecoder* decoder_;
  DemuxerStream* demuxer_stream_;
  TaskRunner* task_runner_;
  std::deque<std::shared_ptr<VideoFrame> > video_frame_queue_;
};

}  // namespace media

#endif