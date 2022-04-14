#ifndef MEDIA_AV_PIPELINE_OBSERVER_H
#define MEDIA_AV_PIPELINE_OBSERVER_H
#include "base/base_type.h"
namespace media {
class AVPipelineObserver {
public:
  virtual void OnGetMediaInfo(const MediaInfo& media_info){};
  virtual void OnPlayProgressUpdate(int timpesatamp){};
};

}
#endif