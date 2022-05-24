#include "media/video_preview/demuxer_unit.h"

#include "log/log_wrapper.h"
#include "media//FFmpeg/ffmpeg_common.h"

namespace media {
static int kFFmpgeAVIOBufferSize = 1024 * 4;
DemuxerUnit::DemuxerUnit(const std::string& source_url)
  :source_url_(source_url){}

void DemuxerUnit::Seek(int64_t timestamp_ms) {
  AUTORUNTIMER("Seek");
  int ret = av_seek_frame(av_format_context_, -1, timestamp_ms * 1000,
                          AVSEEK_FLAG_BACKWARD);
  ClearBuffer();
}

std::shared_ptr<EncodedAVFrame> DemuxerUnit::ReadAVFrame() {
  std::shared_ptr<EncodedAVFrame> new_avframe(new AVPacket());
  bool result = !av_read_frame(av_format_context_, new_avframe.get());
  if (!result) {
    LOGGING(LOG_LEVEL_ERROR) << "av_read_frame failed";
    return nullptr;
  }
  return new_avframe;
}

std::shared_ptr<EncodedAVFrame> DemuxerUnit::ReadVideoAVFrame() {
  AUTORUNTIMER("DemuxerUnit::ReadVideoAVFrame");
  return ReadAVFrameByType(AVMEDIA_TYPE_VIDEO);
}

std::shared_ptr<EncodedAVFrame> DemuxerUnit::ReadAudioAVFrame() {
  return ReadAVFrameByType(AVMEDIA_TYPE_AUDIO);
}

std::shared_ptr<EncodedAVFrame> DemuxerUnit::ReadAVFrameByType(AVMediaType type) {
  std::shared_ptr<EncodedAVFrame> next_av_frame = nullptr;
  while (next_av_frame = ReadAVFrame()) {
    if (next_av_frame == nullptr) {
      break;
    }
    if (stream_index_info[next_av_frame->stream_index] == type) {
      break;
    }
  }
  return next_av_frame;
}

bool DemuxerUnit::Initialize() {
  net::IOChannel* new_data_source = 
    net::IOChannel::CreateIOChannel(source_url_);
  if(!new_data_source) {
    LOGGING(LOG_LEVEL_ERROR) << "Create IO Channel failed! URL("<<source_url_<<").";
    return false;
  }
  data_source_.reset(new_data_source);
  bool result = CreateAVIOContext();
  if(!result) {
    LOGGING(LOG_LEVEL_ERROR) << "Create AVIOContext failed! URL(" << source_url_ << ").";
    return false;
  }

  int error_code;
  error_code = avformat_open_input(&av_format_context_, NULL, NULL, NULL);
  if(error_code != 0) {
    LOGGING(LOG_LEVEL_ERROR) << "avformat_open_input failed! URL(" << source_url_ << ").";
    return false;
  }

  error_code = avformat_find_stream_info(av_format_context_, NULL);
  if (error_code < 0) {
    LOGGING(LOG_LEVEL_ERROR) << "Could not find the stream information";
    return false;
  }
  LOGGING(LOG_LEVEL_ERROR) << "OK: success find the streams information";
  CreateVideoCodecConfig();
  CreateAudioCodecConfig();
  return true;
}

AVStream* DemuxerUnit::GetAVStreamByType(AVMediaType type) {
  AVStream* av_stream = nullptr;
  for (size_t i = 0; i < av_format_context_->nb_streams; i++) {
    AVStream* stream = av_format_context_->streams[i];
    AVCodecContext* av_codec_context = stream->codec;
    AVMediaType media_type = av_codec_context->codec_type;
    if (media_type == type) {
      av_stream = stream;
      break;
    }
  }
  if(av_stream == nullptr) {
    LOGGING(LOG_LEVEL_ERROR) << "Can't find AVStream,type("<<type<<").";
  }
  return av_stream;
}
bool DemuxerUnit::CreateVideoCodecConfig() {
  AVStream* video_av_stream = GetAVStreamByType(AVMEDIA_TYPE_VIDEO);
  if(video_av_stream == nullptr)  {
    return false;
  }
  stream_index_info.insert(std::make_pair(video_av_stream->index, AVMEDIA_TYPE_VIDEO));
  AVStreamToVideoDecoderConfig(video_av_stream, &video_decoder_config_);
  return true;
}

void DemuxerUnit::ClearBuffer() {
  avformat_flush(av_format_context_);
  avio_flush(av_format_context_->pb);
}

bool DemuxerUnit::CreateAudioCodecConfig() {
  AVStream* audio_av_stream = GetAVStreamByType(AVMEDIA_TYPE_AUDIO);
  if (audio_av_stream == nullptr) {
    return false;
  }
  stream_index_info.insert(std::make_pair(audio_av_stream->index, AVMEDIA_TYPE_AUDIO));
  AVStreamToAudioDecoderConfig(audio_av_stream, &audio_decoder_config_);
  return true;
}

VideoDecoderConfig DemuxerUnit::GetVideoConfig() {
  return video_decoder_config_;
}

bool DemuxerUnit::CreateAVIOContext() {
  av_format_context_ = avformat_alloc_context();
  av_io_buffer_ = (unsigned char*)av_malloc(kFFmpgeAVIOBufferSize);
  av_io_context_ = avio_alloc_context(
    av_io_buffer_, kFFmpgeAVIOBufferSize, 
    0, this, FFmpegReadPacketCB, NULL, FFmpegSeekCB);

  av_io_context_->seekable = AVIO_SEEKABLE_NORMAL;
  av_io_context_->write_flag = false;
  // Enable fast, but inaccurate seeks for MP3.
  av_format_context_->flags;  //|= AVFMT_FLAG_FAST_SEEK;
  av_format_context_->pb = av_io_context_;
  return true;
}

int DemuxerUnit::FFmpegReadPacketCB(void* opaque,
                                      unsigned char* buffer,
                                      int buffer_size) {
  DemuxerUnit* demuxer = static_cast<DemuxerUnit*>(opaque);
  return demuxer->FileRead(buffer, buffer_size);
}
int64_t DemuxerUnit::FFmpegSeekCB(void* opaque, int64_t offset, int whence) {
  DemuxerUnit* demuxer = static_cast<DemuxerUnit*>(opaque);
  return demuxer->FileSeek(offset, whence);
}

int DemuxerUnit::FileRead(unsigned char* buffer, int buffer_size) {
  return data_source_->read(buffer, buffer_size);
}

long long DemuxerUnit::FileSeek(long long offset, int whence) {
  if (whence == SEEK_SET) {
    if (offset < 0)
      return -1;
    else
      data_source_->seek(offset);
  } else if (whence == SEEK_CUR) {
    data_source_->seek(data_source_->tell() + offset);
  } else if (whence == SEEK_END) {
    data_source_->seek(data_source_->size());
  } else if (whence == AVSEEK_SIZE) {
    long long result_size = data_source_->size();
    return result_size;
  } else {
    return 0;
  }
  long long result = data_source_->tell();
  return result;
}

} // namespace media