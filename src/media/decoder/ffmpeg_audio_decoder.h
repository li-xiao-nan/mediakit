#ifndef MEDIA_DECODER_FFMPEG_AUDIO_DECODER_H
#define MEDIA_DECODER_FFMPEG_AUDIO_DECODER_H

#include "base/base_type.h"
#include "base/macros.h"
#include "media/decoder/audio_decoder.h"
#include "media/base/audio_decoder_config.h"

namespace media {
class FFmpegAudioDecoder : public AudioDecoder {
 public:
  FFmpegAudioDecoder();
  ~FFmpegAudioDecoder();
  virtual void Initialize(const AudioDecoderConfig& config,
                          InitCB init_cb,
                          OutputCB output_cb) override;
  virtual void Decode(const std::shared_ptr<EncodedAVFrame>,
                      DecodeCB decode_cb) override;
  virtual void ClearBuffer() override;

 private:
  enum DecoderState {
    STATE_UNINITIALIZED,
    STATE_NORMAL,
    STATE_DECODE_COMPLETED,
    STATE_OCCUR_ERROR,
  };

  bool ConfigureDecoder();
  void ReleaseFFmpegResource();
  void FFmpegDecode(const std::shared_ptr<EncodedAVFrame>, DecodeCB decode_cb);

  AVCodecContext* av_codec_context_;
  OutputCB output_cb_;
  AudioDecoderConfig config_;
  DecoderState state_;
  InitCB init_cb_;
  AVFrame* av_frame_;
  DISALLOW_COPY_AND_ASSIGN(FFmpegAudioDecoder);
};
}  // namespace media
#endif