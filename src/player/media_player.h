#ifndef PLAYER_MEDIA_PLAYER_H
#define PLAYER_MEDIA_PLAYER_H
#include <memory>
#include "base/base_type.h"
#include "media/av_pipeline.h"
#include "media/av_pipeline_observer.h"
#include "media/base/video_frame.h"
#include "player/media_player_client.h"
#include "window/video_display_window.h"

namespace mediakit {
class MediaPlayer : public media::AVPipelineObserver{
public:  
  static std::shared_ptr<MediaPlayer> CreateMediaPlayer(
    HWND parent_hwnd, int x, int y, int w, int h, const std::string& video_url);
  static void StopSystem();
  void DisplayCallBack(std::shared_ptr<media::VideoFrame> video_frame);
  void Start();
  void Stop();
  void Seek(int timestamp_ms);
  void Pause();
  void Resume();
  void SetPlaybackRate(float rate);
  void GetVideoKeyFrameAsync(int timestamp_ms, int width, int hegiht);
  void SetClient(MediaPlayerClient* client);
  void UpdatePlayerWindowPosition(int left, int top, int w, int h);
  
  // AVPipelineObserver impl
  void OnGetMediaInfo(const media::MediaInfo& media_info) override;
  void OnPlayProgressUpdate(int timpesatamp) override;
  void OnOpenMediaFileFailed(const std::string file_name,
                             int error_code,
                             const std::string& error_description) override;
  void OnGetKeyVideoFrame(int timestamp_ms,
                          std::shared_ptr<media::VideoFrame> video_frame) override;


 private:
  MediaPlayer(HWND parent_hwnd, const std::string& video_url);
  bool initialize(HWND parent_hwnd, int x, int y, int w, int h);
private:
std::string video_url_;
HWND parent_hwnd_;
MediaPlayerClient* mediaplayer_client_;
std::unique_ptr<media::AVPipeline> av_pipeline_;
std::unique_ptr<mediakit::VideoDisplayWindow> video_display_window_;
};

} // namespace mediakit

#endif