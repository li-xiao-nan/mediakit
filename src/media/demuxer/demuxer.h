#ifndef MEDIA_DEMUXER_DEMUXER_H_
#define MEDIA_DEMUXER_DEMUXER_H_

#include <stdint.h>
#include "demuxer_stream_provider.h"
#include "media/base/pipeline_status.h"

namespace media {
class Demuxer : public DemuxerStreamProvider {
 public:
  Demuxer();
  virtual ~Demuxer();
  virtual void Initialize(PipelineStatusCB status_cb) = 0;
  virtual void Seek(int64_t timestamp, PipelineStatusCB status_cb) = 0;
  virtual void Stop() = 0;
  virtual int64_t GetStartTime() = 0;
  virtual int64_t GetDuration() = 0;
  virtual int64_t GetTimelineOffset() = 0;
  virtual void ShowMediaConfigInfo() = 0;
};

}  // namespace media

#endif