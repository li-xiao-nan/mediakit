#include "player/media_player.h"
#include "media/av_pipeline_factory.h"
#include "boost/bind.hpp"
namespace mediakit {
std::shared_ptr<MediaPlayer> MediaPlayer::CreateMediaPlayer(
  HWND parent_hwnd, int x, int y, int w, int h, const std::string& video_url) {
  std::shared_ptr<MediaPlayer> new_media_player(new MediaPlayer(parent_hwnd, video_url));
  bool result = new_media_player->initialize(parent_hwnd, x, y, w, h);
  new_media_player->av_pipeline_->AddObserver(new_media_player);
  if(!result) {
    new_media_player = nullptr;
  }
  return new_media_player;
}

MediaPlayer::MediaPlayer(HWND parent_hwnd, const std::string& video_url):
  video_url_(video_url),
  parent_hwnd_(parent_hwnd){}

void MediaPlayer::DisplayCallBack(std::shared_ptr<media::VideoFrame> video_frame) {
  if(!video_frame) return;
  if(video_display_window_) video_display_window_->display(video_frame);
}

bool MediaPlayer::initialize(HWND parent_hwnd, int x, int y, int w, int h) {
  video_display_window_.reset(new VideoDisplayWindow(parent_hwnd_, x, y, w, h));
  av_pipeline_ = MakeAVPipeLine(
    video_url_,
    boost::bind(&MediaPlayer::DisplayCallBack, this, _1));
  return true;
}

void MediaPlayer::Start() {
  av_pipeline_->Start();
}

void MediaPlayer::Stop() {
  av_pipeline_->Stop();
}

void MediaPlayer::SetClient(std::shared_ptr<MediaPlayerClient> client) {
  mediaplayer_client_ = client;
}

void MediaPlayer::UpdatePlayerWindowPosition(int left, int top, int w, int h) {
  video_display_window_->SetWindowPosition(left, top, w, h);
}

void MediaPlayer::OnGetMediaInfo(const media::MediaInfo& media_info) {
  if(!mediaplayer_client_) {
    return;
  }
  mediaplayer_client_->OnGetMediaInfo(media_info);
  return;
}

void MediaPlayer::OnPlayProgressUpdate(int timpesatamp) {
  if (!mediaplayer_client_) {
    return;
  }
  mediaplayer_client_->OnPlayProgressUpdate(timpesatamp);
  return;
}

} // namespace mediakit