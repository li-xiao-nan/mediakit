#ifndef MEDIA_VIDEO_RREVIEW_DEMUXER_UNIT_H
#define MEDIA_VIDEO_RREVIEW_DEMUXER_UNIT_H

#include <memory>
#include <string>

#include "base/base_type.h"
#include "net/io_channel.h"
#include "media/base/video_decoder_config.h"
#include "media/base/audio_decoder_config.h"

//  Note by lixiaonan
// 当前使用的FFmpegDemuxer类，耦合过多的播放状态，业务逻辑，以及对运行线程的内置依赖
// 导致当要实现视频帧预览功能时，无法复用FFmpegDemuxer类提供的能力
// 重新设计DemuxerUnit类，目标是提供demuxer相关的基础功能接口，内部不包含播放、运行线程
// 相关的依赖
namespace media {
class DemuxerUnit {
public:
  DemuxerUnit(const std::string& source_url);
  bool Initialize();
  std::shared_ptr<EncodedAVFrame> ReadAVFrame();
  std::shared_ptr<EncodedAVFrame> ReadVideoAVFrame();
  std::shared_ptr<EncodedAVFrame> ReadAudioAVFrame();
  void Seek(int64_t timestamp_ms);
  VideoDecoderConfig GetVideoConfig();
  void ClearBuffer();
private:
  std::shared_ptr<EncodedAVFrame> ReadAVFrameByType(AVMediaType type);
  bool CreateAVIOContext();
  bool CreateVideoCodecConfig();
  bool CreateAudioCodecConfig();
  AVStream* GetAVStreamByType(AVMediaType type);
 // ffmpeg callback function
 static int FFmpegReadPacketCB(void* opaque,
                               unsigned char* buffer,
                               int buffer_size);
 static int64_t FFmpegSeekCB(void* opaque, int64_t offset, int whence);
 int FileRead(unsigned char* buffer, int buffer_size);
 long long FileSeek(long long offset, int whence);
private:
  std::string source_url_;
  // key: stream index
  // value: stream type(video/audio/subtext)
  std::map<int, int> stream_index_info;
  std::unique_ptr<net::IOChannel> data_source_;
  unsigned char* av_io_buffer_;
  AVIOContext* av_io_context_;
  AVFormatContext* av_format_context_;
  VideoDecoderConfig video_decoder_config_;
  AudioDecoderConfig audio_decoder_config_;
};
} // namespace media
#endif