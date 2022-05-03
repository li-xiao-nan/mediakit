#ifndef MEDIA_RENDERER_VIDEO_RENDERER_DELEGATE_H
#define MEDIA_RENDERER_VIDEO_RENDERER_DELEGATE_H

namespace media {
class VideoRendererDelegate {
public:
  virtual void OnPlayClockUpdate(int64_t timestamp) = 0;
};

} // namespace media
#endif