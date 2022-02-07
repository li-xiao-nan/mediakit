#ifndef MEDIA_DEMUXER_DEMUXER_DELEGATE_H_
#define MEDIA_DEMUXER_DEMUXER_DELEGATE_H_

#include <stdint.h>
#include "demuxer_stream_provider.h"
#include "media/base/pipeline_status.h"

namespace media {
  class DemuxerDelegate {
  public:
    // Seek 基于关键帧完成视频播放过程的任意跳转
    // 因此输入的seek时间与实际seek时间相差很大
    // 可以基于次函数通知的时间戳，设置为最终的seek的时间戳
    virtual void OnUpdateAlignedSeekTimestamp(int64_t timestamp) = 0;
  };
}  // namespace media

#endif