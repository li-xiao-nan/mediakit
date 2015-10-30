#ifndef MEDIA_DEMUXER_DEMUXER_STREAM_H_
#define MEDIA_DEMUXER_DEMUXER_STREAM_H_

// DemuxerStream作为向decoder传递信息的媒介，传递如下信息
// 1.demuxer阶段获取到音视频的信息，decoder通过这些信息初始化对应的解码器
// 2.demuder获取的音视频文件中包含的待解码的数据包，

#include "boost/shared_ptr.hpp"

#include "base/base_type.h"
#include "media/base/video_decoder_config.h"
#include "media/base/audio_decoder_config.h"

namespace media {
class DemuxerStream {
 public:
  enum Type {
    AUDIO,
    VIDEO,
    TEXT,
    NONE,
  };

  DemuxerStream();
  virtual ~DemuxerStream();
  virtual Type type() = 0;
  //此函数会返回当前stream类型的数据包。调用者负责保证，将数据包交给正确的解码器进行解码
  virtual void EnqueueEncodedFrame(std::shared_ptr<EncodedAVFrame> frame) = 0;
  virtual std::shared_ptr<EncodedAVFrame> GetNextEncodedFrame() = 0;
  // demuxer state
  virtual void set_demux_complete() = 0;
  virtual bool is_demux_complete() = 0;
  virtual const VideoDecoderConfig& video_decoder_config() const = 0;
  virtual void set_video_decoder_config(
      const VideoDecoderConfig& video_decoder_config) = 0;
  virtual const AudioDecoderConfig& audio_decoder_config() const = 0;
  virtual void set_audio_decoder_config(
      const AudioDecoderConfig& audio_decoder_config) = 0;
};
}  // namespace media

#endif