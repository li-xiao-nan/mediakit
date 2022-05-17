#include "media/decoder/ffmpeg_audio_decoder.h"
#include "media/ffmpeg/ffmpeg_common.h"
#include "log/log_wrapper.h"

namespace media {
FFmpegAudioDecoder::FFmpegAudioDecoder()
  : swr_context_(nullptr),
    av_codec_context_(nullptr),
    av_frame_(nullptr) {}

void FFmpegAudioDecoder::Initialize(const AudioDecoderConfig& config,
                                    InitCB init_cb,
                                    OutputCB output_cb) {
  AUTORUNTIMER("FFmpegAudioDecoder::Initialize");
  config_ = config;
  init_cb_ = init_cb;
  output_cb_ = output_cb;
  bool result = ConfigureDecoder();
  init_cb_(result);
  if (!swr_context_) {
    int in_sample_rate = av_codec_context_->sample_rate;
    int out_sample_rate = av_codec_context_->sample_rate;
    // out_ch_layout:
    // SDL音频播放支持单声道/双声道播放，
    // 单声道音频，依然使用单声道，非单声道音频统一转换为双声道
    // out_sample_fmt: SDL 音频最大支持16位数据类型
    int64_t out_ch_layout =
        (av_codec_context_->channel_layout == AV_CH_LAYOUT_MONO
             ? AV_CH_LAYOUT_MONO
             : AV_CH_LAYOUT_STEREO);
    int out_ch_count = ChannelLayoutToChannelCount(out_ch_layout);
    swr_context_ = swr_alloc_set_opts(NULL, out_ch_layout, AV_SAMPLE_FMT_S16,
      out_sample_rate,
      av_codec_context_->channel_layout,
      av_codec_context_->sample_fmt,
      in_sample_rate, 0, NULL);
      if (!swr_context_ || swr_init(swr_context_) < 0) {
        std::cout << "create the swrContext failed" << std::endl;
        return;
      }
    }
}

FFmpegAudioDecoder::~FFmpegAudioDecoder() {
  ReleaseFFmpegResource();
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
  int avcodec_open_result = avcodec_open2(av_codec_context_, codec, NULL);
  if (!codec || avcodec_open_result < 0) {
    ReleaseFFmpegResource();
    LOGGING(LOG_LEVEL_INFO) << "configure audio decoder failed";
    return false;
  }
  char channel_layout_desc[100] = {0};
  av_get_channel_layout_string(channel_layout_desc, 100, 0, av_codec_context_->channel_layout);
  LOGGING(LOG_LEVEL_INFO) << std::string("audio_codec:")
    << avcodec_get_name(av_codec_context_->codec_id)
    << "; sample_format:" << av_get_sample_fmt_name(av_codec_context_->sample_fmt)
    << "; sample_rate:" << av_codec_context_->sample_rate
    << "; channel:" + av_codec_context_->channels
    << "; channel_layout:" << channel_layout_desc;
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
  AVFrameToAudioFrame(swr_context_, av_frame_, audio_frame, av_codec_context_);
  if (!audio_frame.get()) {
	  state_ = STATE_OCCUR_ERROR;
	  decode_cb(STATUS_DECODE_ERROR);
	  return;
  }
  output_cb_(audio_frame);
  decode_cb(STATUS_OK);
}

void FFmpegAudioDecoder::ClearBuffer() {
  if(!av_codec_context_) return;
  avcodec_flush_buffers(av_codec_context_);
}

}  // namespace media