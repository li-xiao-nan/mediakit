#include "media/decoder/ffmpeg_audio_decoder.h"
#include "media/ffmpeg/ffmpeg_common.h"
#include "log/log_wrapper.h"

namespace media {
FFmpegAudioDecoder::FFmpegAudioDecoder(TaskRunner* task_runner)
    : task_runner_(task_runner) {
}
void FFmpegAudioDecoder::Initialize(const AudioDecoderConfig& config,
                                    InitCB init_cb,
                                    OutputCB output_cb) {
  config_ = config;
  init_cb_ = init_cb;
  output_cb_ = output_cb;
  bool result = ConfigureDecoder();
  init_cb_(result);
}
void FFmpegAudioDecoder::Decode(
    const std::shared_ptr<EncodedAVFrame> encoded_avframe,
    DecodeCB decode_cb) {
  FFmpegDecode(encoded_avframe, decode_cb);
}

bool FFmpegAudioDecoder::ConfigureDecoder() {
  av_codec_context_ = avcodec_alloc_context3(NULL);
  if (!av_codec_context_)
    return false;
  AudioDecoderConfigToAVCodecContext(&config_, av_codec_context_);
  av_codec_context_->refcounted_frames = 0;

  AVCodec* codec = avcodec_find_decoder(av_codec_context_->codec_id);
  if (!codec || avcodec_open2(av_codec_context_, codec, NULL) < 0) {
    ReleaseFFmpegResource();
    std::cout << "configure audio decoder failed" << std::endl;
    return false;
  }
  char channel_layout_desc[100] = {0};
  av_get_channel_layout_string(channel_layout_desc, 100, 0, av_codec_context_->channel_layout);
  LogMessage(LOG_LEVEL_INFO, std::string("audio_codec:") 
    + avcodec_get_name(av_codec_context_->codec_id)
    + "; sample_format:" + av_get_sample_fmt_name(av_codec_context_->sample_fmt)
    + "; sample_rate:" + std::to_string(av_codec_context_->sample_rate)
    + "; channel:" + std::to_string(av_codec_context_->channels)
    + "; channel_layout:" + channel_layout_desc);
  av_frame_ = av_frame_alloc();
  std::cout << "configure audio decoder success!" << std::endl;
  return true;
}

void FFmpegAudioDecoder::ReleaseFFmpegResource() {
  avcodec_free_context(&av_codec_context_);
}

void FFmpegAudioDecoder::FFmpegDecode(
    const std::shared_ptr<EncodedAVFrame> av_packet,
    DecodeCB decode_cb) {
  int decode_count;
  int packet_data_size = av_packet->size;
  
  int result = avcodec_decode_audio4(av_codec_context_, av_frame_,
		  &decode_count, av_packet.get());
  if (result < 0) {
	  state_ = STATE_OCCUR_ERROR;
	  decode_cb(STATUS_DECODE_ERROR);
	  return;
  }

  if (decode_count == 0 && !av_packet.get()) {
	  state_ = STATE_DECODE_COMPLETED;
	  decode_cb(STATUS_DECODE_COMPLETED);
	  return;
  }
  std::shared_ptr<AudioFrame> audio_frame;
  AVFrameToAudioFrame(av_frame_, audio_frame, av_codec_context_);
  if (!audio_frame.get()) {
	  state_ = STATE_OCCUR_ERROR;
	  decode_cb(STATUS_DECODE_ERROR);
	  return;
  }
  output_cb_(audio_frame);
  decode_cb(STATUS_OK);
}

}  // namespace media