#ifndef MEDIA_DEMUXER_DEMUXER_H_
#define MEDIA_DEMUXER_DEMUXER_H_

#include <stdint.h>
#include "demuxer_stream_provider.h"
#include "media/base/pipeline_status.h"
#include "media/demuxer/demuxer_delegate.h"

namespace media {
class Demuxer : public DemuxerStreamProvider {
 public:
  Demuxer()= default;
  virtual ~Demuxer() = default;
  virtual void Initialize(PipelineStatusCB status_cb) = 0;
  virtual void Seek(int64_t timestamp, PipelineStatusCB status_cb) = 0;
  virtual void Stop() = 0;
  virtual void Pause() = 0;
  virtual void Resume() = 0;
  virtual int64_t GetStartTime() = 0;
  virtual int64_t GetDuration() = 0;
  virtual int64_t GetTimelineOffset() = 0;
  virtual void ShowMediaConfigInfo() = 0;
  virtual void SetDelegate(DemuxerDelegate* demuxer_delegate) = 0;
  virtual std::string GetVideoUrl() = 0;
};

}  // namespace media

#endif