#ifndef MEDIA_BASE_TYPE_DEFINE_H
#define MEDIA_BASE_TYPE_DEFINE_H
extern "C" {
#include "libavformat/avformat.h"
#include "libavutil/samplefmt.h"
}
namespace media {
using SampleFormat = AVSampleFormat;
using AudioCodec = AVCodecID;
using VideoCodec = AVCodecID;
// define in avcodec.h
// #define FF_PROFILE_UNKNOWN -99
// #define FF_PROFILE_RESERVED -100
// #define FF_PROFILE_AAC_MAIN 0
// #define FF_PROFILE_AAC_LOW 1
// #define FF_PROFILE_AAC_SSR 2
// #define FF_PROFILE_AAC_LTP 3
// #define FF_PROFILE_AAC_HE 4
// #define FF_PROFILE_AAC_HE_V2 28
// #define FF_PROFILE_AAC_LD 22
// #define FF_PROFILE_AAC_ELD 38
// #define FF_PROFILE_MPEG2_AAC_LOW 128
// #define FF_PROFILE_MPEG2_AAC_HE 131
using VideoCodecProfile = int;
using VideoPixelFormat = AVPixelFormat;
}


#endif // MEDIA_BASE_TYPE_DEFINE_H