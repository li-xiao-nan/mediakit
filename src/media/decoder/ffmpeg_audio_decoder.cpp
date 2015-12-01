
namespace media {
FFmpegAudioDecoder::FFmpegAudioDecoder(TaskRunner* task_runner)
    : task_runner_(task_runner) {}
void FFmpegAudioDecoder::Initialize(const AudioDecoderConfig& config,
                                    InitCB init_cb, OutputCB output_cb) {
  config_ = config;
  init_cb_ = init_cb;
  output_cb_ = output_cb;
  ConfigureDecoder();
}
void FFmpegAudioDecoder::Decode(const std::shared_ptr<EncodedAVFrame>,
                                DecodeCB decode_cb) {}

bool FFmpegAudioDecoder::ConfigureDecoder() {
  av_codec_context_ = avcodec_alloc_context3(NULL);
  if (!av_codec_context_) return false;
  AudioDecoderConfigToAVCodecContext(&config_, av_codec_context_);

  av_codec_context_->thread_count = kDecodeThreads;
  av_codec_context_->thread_type =
      low_delay ? FF_THREAD_SLICE : FF_THREAD_FRAME;
  av_codec_context_->flags |= CODEC_FLAG_EMU_EDGE;
  av_codec_context_->refcounted_frames = 0;

  AVCodec* codec = avcodec_find_decoder(av_codec_context_->codec_id);
  if (!codec || avcodec_open2(av_codec_context_, codec, NULL) < 0) {
    ReleaseFFmpegResource();
    std::cout << "configure audio decoder failed" << std::endl;
    return false;
  }
  av_frame_ = av_frame_alloc();
  std::cout << "configure audio decoder success!" << std::endl;
  return true;
}

void FFmpegAudioDecoder::ReleaseFFmpegResource() {
  avcodec_free_context(&av_codec_context_);
}

void FFmpegAudioDecoder::FFmpegDecode(
    const std::shared_ptr<EncodedAVFrame> av_packet, DecodeCB decode_cb) {
  int decode_count;
  int result = avcodec_decode_audio4(_audioCodecCtx, av_frame_, &decode_count,
                                     av_packet.get());
  if (result < 0) {
    state_ = STATE_OCCUR_ERROR;
    decode_cb(STATUS_DECODE_ERROR);
    return;
  }

  if (decode_count == 0 && !encoded_avframe.get()) {
    state_ = STATE_DECODE_COMPLETED;
    decode_cb(STATUS_DECODE_COMPLETED);
  }

  std::shared_ptr<AudioFrame> audio_frame;
}

}  // namespace media