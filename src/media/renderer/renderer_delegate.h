#ifndef MEDIA_RENDERER_DELEGATE_H
#define MEDIA_RENDERER_DELEGATE_H
namespace media {
class RendererDelegate {
public:
  // unit:ms
  virtual void OnPlayProgressUpdate(int timestamp) = 0;
};
} // namespace media
#endif