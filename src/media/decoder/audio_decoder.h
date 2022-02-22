#ifndef MEDIA_DECODER_AUDIO_DECODER_H
#define MEDIA_DECODER_AUDIO_DECODER_H

#include "boost/function.hpp"

#include "media/base/audio_frame.h"
#include "base/base_type.h"

namespace media {
class AudioDecoderConfig;

class AudioDecoder {
 public:
  enum Status {
    STATUS_OK,
    STATUS_DECODE_ABORT,
    STATUS_DECODE_ERROR,
    STATUS_DECODE_COMPLETED,
  };

  typedef boost::function<void(bool result)> InitCB;
  typedef boost::function<void(Status)> DecodeCB;
  typedef boost::function<void(std::shared_ptr<AudioFrame> audio_frame)>
      OutputCB;

  virtual ~AudioDecoder() = default;

  virtual void Initialize(const AudioDecoderConfig& config,
                          InitCB init_cb,
                          OutputCB output_cb) = 0;
  virtual void Decode(const std::shared_ptr<EncodedAVFrame>,
                      DecodeCB decode_cb) = 0;
  virtual void ClearBuffer() = 0;
};

}  // namespace media

#endif