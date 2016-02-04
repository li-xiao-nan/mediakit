#include "audio_renderer_impl.h"
#include "media/renderer/sdl_audio_renderer_sink.h"

namespace media {

int kMaxTimeDelta = 100; //ms

AudioRendererImpl::AudioRendererImpl(
    TaskRunner* task_runner,
    const AudioFrameStream::VecAudioDecoders& vec_audio_decoders)
    : state_(STATE_UNINITIALIZED) {
  task_runner_ = task_runner;
  pending_read_ = false;
  audio_frame_stream_ = new AudioFrameStream(task_runner, vec_audio_decoders);
  audio_renderer_sink_ = new SdlAudioRendererSink();
}

void AudioRendererImpl::Initialize(DemuxerStream* demuxer_stream,
                                   PipelineStatusCB init_cb,
                                   PipelineStatusCB status_cb,
                                   GetTimeCB get_time_cb) {
  demuxer_stream_ = demuxer_stream;
  init_cb_ = init_cb;
  status_cb_ = status_cb;
  get_time_cb_ = get_time_cb;
  InitAudioRendererFlow(true);
}

void AudioRendererImpl::InitAudioRendererFlow(bool result) {
  if (!result) {
      init_cb_(AUDIO_RENDERER_INIT_FAILED);;
  }
  AudioRendererSink::InitCB sub_object_init_cb =
      boost::bind(&AudioRendererImpl::InitAudioRendererFlow, this, _1);
  state_ = GetNextState();
  switch (state_) {
    case STATE_INIT_DECODER:
      audio_frame_stream_->Initialize(demuxer_stream_, sub_object_init_cb);
      break;
    case STATE_INIT_SINKER: {
        AudioDecoderConfig audio_decoder_config =
            demuxer_stream_->audio_decoder_config();
        InitAudioRenderSink(audio_decoder_config, sub_object_init_cb);

    } break;
    case STATE_PLAYING:
      init_cb_(PIPELINE_OK);
      break;
    default:
      return;
  }
}

AudioRendererImpl::State AudioRendererImpl::GetNextState() {
  switch (state_) {
    case STATE_UNINITIALIZED:
      return STATE_INIT_DECODER;
    case STATE_INIT_DECODER:
      return STATE_INIT_SINKER;
    case STATE_INIT_SINKER:
    case STATE_FLUSHING:
      return STATE_PLAYING;
    default:
      return STATE_PLAYING;
  }
}

void AudioRendererImpl::StartPlayingFrom(int64_t offset) {
  audio_renderer_sink_->Start();
}

void AudioRendererImpl::SetPlaybackRate(float rate) {
}

void AudioRendererImpl::SetVolume(float volume) {
}

void AudioRendererImpl::Render(uint8_t* data, int data_size) {

	static int pre_timestamp = 0;
	int64_t new_timestamp = get_time_cb_();
	pre_timestamp = new_timestamp - pre_timestamp;
  memset(data, 0, data_size);
  ReadReadyFrameLocked();
  if (pending_paint_frames_.empty())
    return;
  int size = pending_paint_frames_.size();
  int needed_data_size = data_size;
  int buffer_cursor = 0;
  for (int i = 0; i < size; i++) {
    std::shared_ptr<AudioFrame> next_audio_frame =
        pending_paint_frames_.front();
    int read_count =
        next_audio_frame->Read(data + buffer_cursor, needed_data_size);
    buffer_cursor += read_count;
    needed_data_size -= read_count;
    if (next_audio_frame->data_size() == 0)
      pending_paint_frames_.pop();
    if (needed_data_size == 0)
      break;
  }

}

void AudioRendererImpl::OnRenderError() {
}

void AudioRendererImpl::ReadFrameIfNeeded() {
  if (pending_read_ || ready_audio_frames_.size() >= 4)
    return;
  audio_frame_stream_->Read(
      boost::bind(&AudioRendererImpl::OnReadFrameDone, this, _1, _2));
}

void AudioRendererImpl::OnReadFrameDone(
    AudioFrameStream::Status status,
    std::shared_ptr<AudioFrame> audio_frame) {
  boost::mutex::scoped_lock lock(frame_queue_mutex_);
  if (audio_frame.get())
    ready_audio_frames_.push(audio_frame);
  pending_read_ = false;
}

void AudioRendererImpl::ReadReadyFrameLocked() {
  ReadFrameIfNeeded();
  boost::mutex::scoped_lock lock(frame_queue_mutex_);
  static int64_t pre_timestamp;
  int64_t current_time = get_time_cb_();
  pre_timestamp = current_time;
  for (int i = 0; i < ready_audio_frames_.size(); i++) {
    std::shared_ptr<AudioFrame> next_audio_frame = ready_audio_frames_.front();
    int64_t next_frame_pts = next_audio_frame->pts();
    if (next_frame_pts > current_time)
      return;
    int64_t time_delta = current_time - next_frame_pts;
    if (time_delta < kMaxTimeDelta) {
      pending_paint_frames_.push(next_audio_frame);
    }
    ready_audio_frames_.pop();
  }
}

void AudioRendererImpl::InitAudioRenderSink(const AudioDecoderConfig& audio_decoder_config,
    AudioRendererSink::InitCB init_cb){
    AudioParameters audio_parameters;
    audio_parameters.sample_rate_ = audio_decoder_config.sample_rate();
    audio_parameters.channel_count_ = audio_decoder_config.channel_count();
    audio_parameters.sample_format_ = audio_decoder_config.sample_format();
    audio_renderer_sink_->Initialize(this, init_cb,
        audio_parameters);
}

}  // namespace media