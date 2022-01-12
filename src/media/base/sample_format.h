//
//  sample_format.h
//  mediacore
//
//  Created by 李楠 on 15/10/6.
//
//

#ifndef MEDIA_BASE_SAMPLE_FORMAT_H
#define MEDIA_BASE_SAMPLE_FORMAT_H
extern "C" {
#include "libavutil/samplefmt.h"
}
namespace media {
  using SampleFormat = AVSampleFormat;
}  // namespace media

#endif
