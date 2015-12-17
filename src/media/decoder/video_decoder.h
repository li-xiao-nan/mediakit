#ifndef MEDIA_DECODER_VIDEO_DECODER_H
#define MEDIA_DECODER_VIDEO_DECODER_H

#include "boost/function.hpp"

#include "media/base/video_decoder_config.h"
#include "media/base/video_frame.h"
#include "base/base_type.h"

namespace media {
class VideoDecoder {
 public:
  enum Status {
    STATUS_OK,
    STATUS_DECODE_ABORT,
    STATUS_DECODE_ERROR,
    STATUS_DECODE_COMPLETED,
  };

  typedef boost::function<void(bool result)> InitCB;
  typedef boost::function<void(Status)> DecodeCB;
  typedef boost::function<void(std::shared_ptr<VideoFrame> video_frame)>
      OutputCB;

  virtual ~VideoDecoder() {}

  virtual void Initialize(const VideoDecoderConfig& config,
                          InitCB init_cb,
                          OutputCB output_cb) = 0;
  virtual void Decode(const std::shared_ptr<EncodedAVFrame>,
                      DecodeCB decode_cb) = 0;
};
}
#endif