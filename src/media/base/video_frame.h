#ifndef MEDIA_BASE_VIDEO_FRAME_H
#define MEDIA_BASE_VIDEO_FRAME_H

#include <stdint.h>
#include <iostream>

namespace media {
class VideoFrame {
 public:
     class TimeRecoder{
     public:
         int _frameNo;
         int _decodeExpendTime;
         int _addQueueTime;
         int _displayTime;
     };

  enum Format {
    UNKNOWN,
    YUV,
    RGB,
    ARGB,
    FORMAT_MAX,
  };

  VideoFrame(int w, int h, Format format);
  ~VideoFrame();

  unsigned char* begin() const { return _data; }

  unsigned int _w;
  unsigned int _h;
  Format _format;
  unsigned char* _data;
  unsigned char* _yuvData[3];
  unsigned char* _yuv;
  int _yuvStride[3];
  int _yuvLineCnt[3];
  int64_t _pts;
  int64_t timestamp_;
  TimeRecoder _timeRecoder;
};
}  // namespace media

#endif