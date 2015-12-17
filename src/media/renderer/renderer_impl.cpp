#include "renderer_impl.h"

#include "boost/bind.hpp"

#include "media/base/wall_clock_time_source_impl.h"

namespace media {
RendererImpl::RendererImpl(TaskRunner* task_runner,
                           std::shared_ptr<AudioRenderer> audio_renderer,
                           std::shared_ptr<VideoRenderer> video_renderer)
    : task_runner_(task_runner),
      state_(STATE_UNINITIALIZED),
      time_source_(new WallClockTimeSourceImpl()),
      audio_renderer_(audio_renderer),
      video_renderer_(video_renderer) {
}

void RendererImpl::Initialize(DemuxerStreamProvider* demuxer_stream_provider,
                              PipelineStatusCB init_cb,
                              PipelineStatusCB status_cb,
                              VideoRenderer::PaintCB paint_cb) {
  demuxer_stream_provider_ = demuxer_stream_provider;
  paint_cb_ = paint_cb;
  init_cb_ = init_cb;
  status_cb_ = status_cb;

  InitializeAudioRenderer();
}

void RendererImpl::StartPlayingFrom(int64_t time_offset) {
  if (state_ != STATE_NORMAL)
    return;
  time_source_->SetStartTime(time_offset);
  video_renderer_->StartPlayingFrom(time_offset);
  audio_renderer_->StartPlayingFrom(time_offset);
  time_source_->StartTicking();

  // todo(lixiaonan) 实现 audioRenderer实现播放的部分
  // audio_renderer_->StartPlayingFrome(time_offset);
}
void RendererImpl::SetPlaybackRate(float rate) {
}
void RendererImpl::SetVolume(float volume) {
}

void RendererImpl::InitializeAudioRenderer() {
  audio_renderer_->Initialize(
      demuxer_stream_provider_->GetDemuxerStream(DemuxerStream::AUDIO),
      boost::bind(&RendererImpl::OnInitializeAudioRendererDone, this, _1),
      status_cb_, boost::bind(&RendererImpl::GetCurrentTime, this));
}
void RendererImpl::OnInitializeAudioRendererDone(PipelineStatus status) {
  // TODO(lixiaonan):
  if (status != PIPELINE_OK) {
    init_cb_(status);
    return;
  }
  std::cout << "Audio Renderer init Done" << std::endl;
  InitializeVideoRenderer();
}

void RendererImpl::InitializeVideoRenderer() {
  video_renderer_->Initialize(
      demuxer_stream_provider_->GetDemuxerStream(DemuxerStream::VIDEO),
      boost::bind(&RendererImpl::OnInitializeVideoRendererDone, this, _1),
      status_cb_, paint_cb_, boost::bind(&RendererImpl::GetCurrentTime, this));
}

void RendererImpl::OnInitializeVideoRendererDone(PipelineStatus status) {
  // TODO(lixiaonan):增加 init audio renderer的初始化状态处理
  state_ = status == PIPELINE_OK ? STATE_NORMAL : STATE_NORMAL;
  init_cb_(status);
}

int64_t RendererImpl::GetCurrentTime() {
  return time_source_->GetCurrentMediaTime();
}

}  // namespace media