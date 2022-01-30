#ifndef MEDIA_BASE_VIDEO_FRAME_H
#define MEDIA_BASE_VIDEO_FRAME_H

#include <stdint.h>
#include <iostream>
#include "log/log_wrapper.h"

namespace media {
class VideoFrame {
 public:
     class TimeRecoder{
     public:
        void LogTimeRecorder() {
          LogMessage(LOG_LEVEL_INFO, std::string("FFmpegVideoDecoder::Decode; ") + 
            "FrameNo:" + std::to_string(_frameNo)
            + " decodeExpendTime:" + std::to_string(_decodeExpendTime)
            + " addQueueTime:" + std::to_string(_addQueueTime)
            + " pst:" + std::to_string(_pst));
        }
     public:
         int _frameNo;
         int _decodeExpendTime;
         int _addQueueTime;
         int64_t _pst;
         int64_t _popupTime;
         std::wstring _renderResult;
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

  static int frame_count_base_;
private:
  int GetNextFrameNo();

public:
  int frame_no_;
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