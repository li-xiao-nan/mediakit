#ifndef COMMON_DEF_H
#define COMMON_DEF_H
#include <stdint.h>

#include "boost/function.hpp"
#include "boost/scoped_ptr.hpp"
#include "boost/asio/io_service.hpp"
#include "boost/assert.hpp"
#include "timer/ClockTime.h"
#include "log/log_wrapper.h"

extern "C" {
#ifndef INT64_C
#define INT64_C(c) (c##LL)
#endif

#ifndef UINT64_C
#define UINT64_C(c) (c##ULL)
#endif

#include <libavutil/imgutils.h>
#include <libavutil/samplefmt.h>
#include <libavutil/mathematics.h>
#include <libavformat/avformat.h>
#include <libavformat/avio.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
}

namespace media {
#define EncodedAVFrame AVPacket
typedef boost::function<void(void)> AsyncTask;
typedef boost::function<void(void)> CallBackFunc;
typedef boost::asio::io_service TaskRunner;
struct MediaInfo {
  // unit: ms
  int64_t video_duration_;
};

class FunctionWrap {
public:
  FunctionWrap(AsyncTask task, const std::string& post_from,
    bool show_debug_info, CallBackFunc callback)
    : task_(task)
    , post_timestamp_(MediaCore::getTicks())
    , post_from_(post_from)
    , show_debug_info_(show_debug_info)
    , run_completed_callback_(callback) {}

  void operator()(void) {
    before_run_timestamp_ = MediaCore::getTicks();
    task_();
    after_run_timestamp_ = MediaCore::getTicks();
    if(show_debug_info_) {
      LOGGING(LOG_LEVEL_DEBUG)
        <<"EnQueue-time:"<< before_run_timestamp_ - post_timestamp_
        <<"; Run-time:"<< after_run_timestamp_ - before_run_timestamp_
        << "; post-from:"<<post_from_;
    }
    if(run_completed_callback_) {
      run_completed_callback_();
    }
  }
private:
  int64_t post_timestamp_;
  int64_t before_run_timestamp_;
  int64_t after_run_timestamp_;
  CallBackFunc run_completed_callback_;
  bool show_debug_info_;
  std::string post_from_;
  AsyncTask task_;
};
}
#endif