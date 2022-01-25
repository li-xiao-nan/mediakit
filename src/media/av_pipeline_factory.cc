#include "media/av_pipeline_factory.h"

#include "media/decoder/ffmpeg_audio_decoder.h"
#include "media/decoder/ffmpeg_video_decoder.h"
#include "media/renderer/video_renderer_impl.h"
#include "media/renderer/audio_renderer_impl.h"
#include "media/demuxer/ffmpeg_demuxer.h"
#include "net/url.h"
#include "net/io_channel.h"
#include "log/log_wrapper.h"

namespace media {
void SeekCB(media::PipelineStatus status) {
  switch (status) {
    case media::PIPELINE_ERROR_SEEK_FAILED:
      std::cout << "seek failed" << std::endl;
      break;
    case media::PIPELINE_OK:
      std::cout << "seek success" << std::endl;
      break;
  }
}

void PipelineStatusCallBack(media::PipelineStatus status) {
  switch (status) {
    case media::PIPELINE_OK:
      std::cout << "pipeline_ok" << std::endl;
      break;
    case media::PIPELINE_ERROR_SEEK_FAILED:
      std::cout << "occur error: PIPELINE_ERROR_SEEK_FAILED" << std::endl;
      break;
    case media::DEMUXER_OPEN_CONTEXT_FAILED:
      std::cout << "occur error: DEMUXER_OPEN_CONTEXT_FAILED" << std::endl;
      break;
    case media::DEMUXER_FIND_STREAM_INFO_FAILED:
      std::cout << "occur error: DEMUXER_OPEN_CONTEXT_FAILED" << std::endl;
      break;
    default:
      std::cout << "unknown error" << std::endl;
  }
}

std::unique_ptr<AVPipeline> MakeAVPipeLine(const std::string& url,
                                           VideoRenderer::PaintCB paint_cb) {
  std::shared_ptr<net::IOChannel> data_source(
      net::IOChannel::CreateIOChannel(net::Url(url)));
  if (data_source == nullptr) {
    media::LogMessage(media::LOG_LEVEL_ERROR,
                      "DataSource Create Failed, Original Url:" + url);
    return nullptr;
  }
  std::shared_ptr<media::Demuxer> demuxer =
      std::make_shared<media::FFmpegDemuxer>(data_source);
  std::shared_ptr<media::Renderer> renderer = MakeRenderer();
  std::unique_ptr<AVPipeline> av_pipeline_instance =
      std::make_unique<AVPipeline>(demuxer, renderer,
                                   boost::bind(&PipelineStatusCallBack, _1),
                                   boost::bind(&SeekCB, _1), paint_cb);
  return av_pipeline_instance;
}

std::shared_ptr<media::Renderer> MakeRenderer() {
  std::shared_ptr<media::Renderer> renderer;
  std::vector<media::VideoDecoder*> vector_video_decoder;
  media::VideoDecoder* video_decoder = new media::FFmpegVideoDecoder();
  vector_video_decoder.push_back(video_decoder);

  media::AudioDecoder* audio_decoder = new media::FFmpegAudioDecoder();
  std::vector<media::AudioDecoder*> vector_audio_decoder;
  vector_audio_decoder.push_back(audio_decoder);

  std::shared_ptr<media::VideoRenderer> video_renderer(
      new media::VideoRendererImpl(vector_video_decoder));
  std::shared_ptr<media::AudioRenderer> audio_renderer(
      new media::AudioRendererImpl(vector_audio_decoder));
  renderer.reset(new media::RendererImpl(audio_renderer, video_renderer));
  return renderer;
}
} // namespace media