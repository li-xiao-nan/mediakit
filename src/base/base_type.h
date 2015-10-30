#ifndef COMMON_DEF_H
#define COMMON_DEF_H
#include <stdint.h>
#include "boost/function.hpp"
#include "boost/scoped_ptr.hpp"
#include "boost/asio/io_service.hpp"
extern "C" {
#ifndef INT64_C
#define INT64_C(c) (c##LL)
#endif

#ifndef UINT64_C
#define UINT64_C(c) (c##ULL)
#endif

#ifndef PRId64
#define PRId64 "lld"
#endif

#include <libavutil/imgutils.h>
#include <libavutil/samplefmt.h>
#include <libavutil/timestamp.h>
#include <libavutil/mathematics.h>
#include <libavformat/avformat.h>
#include <libavformat/avio.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
}

#include <stdint.h>

namespace base {
#define ENABLE_USE_FFMPEG 1

class MediaInfo {
 public:
  int _videoWidth;
  int _videoHeight;
  int64_t _videoDuration;  // millseconds
  double _videoFramerate;
  int32_t _key_frame_count;
  double _AudioSamplerate;
  uint8_t _channels;
  int _sample_rate;
  AVRational _audioTimeBase;
  AVRational _videoTimeBase;
};

enum PlayState {
  PLAY_STATE_PLAYING,
  PLAY_STATE_BUFFERING,
  PLAY_STATE_STOP,
  PLAY_STATE_OVER,
  PLAY_STATE_PAUSE
};

enum MediaParserState {
  PARSER_STATE_PARSING,
  PARSER_STATE_PAUSE,
  PARSER_STATE_COMPLETE,
};

typedef boost::function<void(void)> AsyncTask;

/*
 class EncodedAVFrame{
 public:
 EncodedAVFrame();
 ~EncodedAVFrame();
 uint8_t* GetData() const;
 void SetData()

 private:
 boost::scoped_ptr<AVPacket> avpacket_;
 };

 EncodedAVFrame::EncodedAVFrame()
 : avpacket_(new AVPacket){

 }
 */
#if 1
#define EncodedAVFrame AVPacket
#endif
}

namespace media {
typedef boost::function<void(void)> AsyncTask;
typedef boost::asio::io_service TaskRunner;
};
#endif