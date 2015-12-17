#ifndef MEDIA_BASE_AUDIO_PARAMTERS_H
#define MEDIA_BASE_AUDIO_PARAMTERS_H

#include "media/base/sample_format.h"
namespace media {
struct AudioParameters {
  int sample_rate_;
  int channel_count_;
  SampleFormat sample_format_;
};
}  // namespace media
#endif