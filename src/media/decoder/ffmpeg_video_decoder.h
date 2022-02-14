#ifndef MEDIA_DECODER_FFMPEG_VIDEO_DECODER_H
#define MEDIA_DECODER_FFMPEG_VIDEO_DECODER_H

#include "media/decoder/video_decoder.h"
#include "base/base_type.h"
#include "media/base/video_decoder_config.h"
#include "media/base/pipeline_status.h"

namespace media {
// Always try to use three threads for video decoding.  There is little reason
// not to since current day CPUs tend to be multi-core and we measured
// performance benefits on older machines such as P4s with hyperthreading.
//
// Handling decoding on separate threads also frees up the pipeline thread to
// continue processing. Although it'd be nice to have the option of a single
// decoding thread, FFmpeg treats having one thread the same as having zero
// threads (i.e., avcodec_decode_video() will execute on the calling thread).
// Yet another reason for having two threads :)
static const int kDecodeThreads = 2;
static const int kMaxDecodeThreads = 16;

class FFmpegVideoDecoder : public VideoDecoder {
 public:
  FFmpegVideoDecoder();
  virtual ~FFmpegVideoDecoder();

  virtual void Initialize(const VideoDecoderConfig& config, InitCB init_cb,
                          OutputCB output_cb);
  virtual void Decode(const std::shared_ptr<EncodedAVFrame>,
                      DecodeCB decode_cb);
  virtual void ClearBuffer() override; 

 private:
  enum DecoderState {
    STATE_UNINITIALIZED,
    STATE_NORMAL,
    STATE_DECODE_COMPLETED,
    STATE_OCCUR_ERROR,
  };

  bool ConfigureDecoder(bool low_delay);
  void ReleaseFFmpegResource();
  int64_t GetPlaybackTime();

  AVCodecContext* av_codec_context_;
  OutputCB output_cb_;
  VideoDecoderConfig config_;
  DecoderState state_;
  AVFrame* av_frame_;
};
}  // namespace media
#endif