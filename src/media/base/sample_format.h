//
//  sample_format.h
//  mediacore
//
//  Created by 李楠 on 15/10/6.
//
//

#ifndef MEDIA_BASE_SAMPLE_FORMAT_H
#define MEDIA_BASE_SAMPLE_FORMAT_H

namespace media {

enum SampleFormat {
  SAMPLE_FORMAT_UNKNOWN = 0,
  SAMPLE_FORMAT_U8,
  SAMPLE_FORMAT_S16,
  SAMPLE_FORMAT_S32,
  SAMPLE_FORMAT_PLANAR_S16,
  SAMPLE_FORMAT_PLANAR_F32,
  SAMPLE_FORMAT_PLANAR_S32,
  SAMPLE_FORMAT_MAX = SAMPLE_FORMAT_PLANAR_S32,
};

}  // namespace media

#endif
