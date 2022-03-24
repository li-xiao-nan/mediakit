#ifndef PLAYER_MEDIA_PLAYER_H
#define PLAYER_MEDIA_PLAYER_H
#include <memory>
#include "media/av_pipeline.h"
#include "window/video_display_window.h"

namespace mediakit {
class MediaPlayer {
public:  
  static MediaPlayer* CreateMediaPlayer(
    HWND parent_hwnd, int x, int y, int w, int h, const std::string& video_url);
  void DisplayCallBack(std::shared_ptr<media::VideoFrame> video_frame);
  void Start();
  void Stop();
private:
  MediaPlayer(HWND parent_hwnd, const std::string& video_url);
  bool initialize(HWND parent_hwnd, int x, int y, int w, int h);
private:
std::string video_url_;
HWND parent_hwnd_;
std::unique_ptr<media::AVPipeline> av_pipeline_;
std::unique_ptr<mediakit::VideoDisplayWindow> video_display_window_;
};

} // namespace mediakit

#endif