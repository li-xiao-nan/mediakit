#ifndef PLAYER_MEDIA_PLAYER_CLIENT_H
#define PLAYER_MEDIA_PLAYER_CLIENT_H
#include "base/base_type.h"
namespace mediakit{
class MediaPlayerClient{
public:
  virtual void OnGetMediaInfo(const media::MediaInfo& media_info) = 0;
  virtual void OnPlayProgressUpdate(const int64_t timestamp) =0;
  virtual void OnPlayStateChanged() = 0;
};
} // namespace mediakit
#endif