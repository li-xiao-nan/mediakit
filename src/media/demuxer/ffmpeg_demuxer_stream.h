//
//  ffmpeg_demuxer_stream.h
//  mediacore
//
//  Created by 李楠 on 15/8/31.
//
//

#ifndef MEDIA_DEMUXER_FFMPEG_DEMUXER_STREAM_H
#define MEDIA_DEMUXER_FFMPEG_DEMUXER_STREAM_H

#include <queue>

#include "boost/scoped_ptr.hpp"
#include "base/macros.h"
#include "media/demuxer/demuxer_stream.h"

namespace media {
class FFmpegDemuxer;
class AudioDecoderConfig;
class VideoDecoderConfig;

class FFmpegDemuxerStream : public DemuxerStream {
 public:
  FFmpegDemuxerStream(FFmpegDemuxer* demuxer, AVStream* stream);

  // DemuxerStream implementation
  virtual Type type();
  virtual void EnqueueEncodedFrame(
      std::shared_ptr<EncodedAVFrame> frame) override;
  virtual void Read(ReadCB read_cb) override;
  virtual void set_demux_complete() override;
  virtual bool is_demux_complete() override;
  virtual const VideoDecoderConfig& video_decoder_config() const override;
  virtual void set_video_decoder_config(
      const VideoDecoderConfig& video_decoder_config) override;
  virtual const AudioDecoderConfig& audio_decoder_config() const override;
  virtual void set_audio_decoder_config(
      const AudioDecoderConfig& audio_decoder_config) override;
  virtual void ClearEncodedAVFrameBuffer() override;
  virtual void ShowStateInfo() override;

  bool HasAvailableCapacity();
  int stream_index() { return stream_index_; }
  int64_t duration() { return duration_; }
  const AVRational& time_base() { return time_base_; }
  void ShowConfigInfo();

 private:
  int stream_index_;
  // micro second
  int64_t duration_;
  AVRational time_base_;
  bool is_demux_complete_;
  Type type_;
  ReadCB read_cb_;
  AudioDecoderConfig audio_decoder_config_;
  VideoDecoderConfig video_decoder_config_;
  FFmpegDemuxer* demuxer_;
  std::queue<std::shared_ptr<EncodedAVFrame> > encoded_avframe_queue_;
  DISALLOW_COPY_AND_ASSIGN(FFmpegDemuxerStream);
};

}  // namespace media
#endif /* defined(__mediacore__ffmpeg_demuxer_stream__) */
