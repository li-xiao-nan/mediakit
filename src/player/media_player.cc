#include "player/media_player.h"
#include "media/av_pipeline_factory.h"
#include "boost/bind.hpp"
namespace mediakit {
MediaPlayer* MediaPlayer::CreateMediaPlayer(const std::string& video_url) {
  MediaPlayer* new_media_player = new MediaPlayer(video_url);
  bool result = new_media_player->initialize();

  if(!result) {
    delete new_media_player;
    new_media_player = nullptr;
  }
  
  return new_media_player;
}

MediaPlayer::MediaPlayer(const std::string& video_url):video_url_(video_url) {}

void MediaPlayer::DisplayCallBack(std::shared_ptr<media::VideoFrame> video_frame) {
  if(!video_frame) return;
  if(video_display_window_) video_display_window_->display(video_frame);
}

bool MediaPlayer::initialize() {
  video_display_window_.reset(new VideoDisplayWindow());
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

} // namespace mediakit