#ifndef COMMON_DEF_H
#define COMMON_DEF_H
#include <stdint.h>

#include "boost/function.hpp"
#include "boost/scoped_ptr.hpp"
#include "boost/asio/io_service.hpp"
#include "boost/assert.hpp"

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
#include <libavutil/mathematics.h>
#include <libavformat/avformat.h>
#include <libavformat/avio.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
}

#include <stdint.h>

namespace media {
#define EncodedAVFrame AVPacket
typedef boost::function<void(void)> AsyncTask;
typedef boost::asio::io_service TaskRunner;
};
#endif