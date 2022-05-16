#include "media/video_preview/video_preview_pipeline.h"
#include "base/message_loop_thread_manager.h"
#include <mutex>

namespace media {
VideoPreviewPipeline::VideoPreviewPipeline(const std::string& video_url)
  : running_(true)
  , initialized_(false)
  ,video_url_(video_url)
  , demuxer_unit_(new DemuxerUnit(video_url))
  , video_decoder_unit_(new VideoDeocderUnit())
  , delegate_(nullptr)
  , working_(false){}

bool VideoPreviewPipeline::initialize() {
  bool result = false;
  
  result = demuxer_unit_->Initialize();
  if(!result) return result;

  result = video_decoder_unit_->initialize(demuxer_unit_->GetVideoConfig());
  if(!result) return result;
  initialized_ = true;
  return result;
}

void VideoPreviewPipeline::SetDelegate(VideoPreviewPipelineDelegate* delegate) {
  delegate_ = delegate;
}

void VideoPreviewPipeline::GetKeyFrame(int64_t timestamp_ms,
  int width, int height) {
  AsyncTask task = 
    boost::bind(&VideoPreviewPipeline::GetKeyFrameInternal, this,
      timestamp_ms, width, height);
  task_queue_mutex_.lock();
  if(working_) {
    pending_task_ = task;
  } else {
    working_ = true;
    PostTaskDebug(TID_WORK, task);
  }
  task_queue_mutex_.unlock();

}

// run on work thread
void VideoPreviewPipeline::GetKeyFrameInternal(int64_t timestamp_ms,
  int width, int hegiht) {
  if(!initialized_) {
    return;
  }
  AUTORUNTIMER(__FUNCTION__);
  int64_t begin = MediaCore::getTicks();
  LOGGING(LOG_LEVEL_DEBUG)<<"start:"<< begin;
  demuxer_unit_->Seek(timestamp_ms);
  video_decoder_unit_->ClearBuffer();
  LOGGING(LOG_LEVEL_DEBUG) << MediaCore::getTicks();
  std::shared_ptr<VideoFrame> video_frame = nullptr;
  // 视频解码过程，不一定每次都能返回解码结果，初始阶段一般会依赖4-5个
  // EncodedAVframe数据，才能完整初次解码过程
  LOGGING(LOG_LEVEL_DEBUG) << MediaCore::getTicks();
  while(running_) {
    std::shared_ptr<EncodedAVFrame> next_key_avframe =
        demuxer_unit_->ReadVideoAVFrame();
    if (next_key_avframe == nullptr) {
      break;
    }
    video_frame = video_decoder_unit_->Decode(next_key_avframe, width, hegiht);
    if(!video_frame){
      continue;
    } else {
      break;
    }
  }
  LOGGING(LOG_LEVEL_DEBUG) << "while(true):" << MediaCore::getTicks() - begin;
  if(!video_frame) {
    return;
  }

  if(delegate_) {
    AsyncTask task = boost::bind(
      &VideoPreviewPipelineDelegate::OnGetKeyVideoFrame,
      delegate_, timestamp_ms, video_frame);
    POSTTASK(TID_MAIN, task, true);
  }
  task_queue_mutex_.lock();
  if(!pending_task_) {
    working_ = false;
  } else {
    POSTTASK(TID_WORK, pending_task_, true);
    pending_task_ = (boost::function<void(void)>)0;
  }
  task_queue_mutex_.unlock();
  LOGGING(LOG_LEVEL_DEBUG) << "end:" << MediaCore::getTicks() - begin;
}

void VideoPreviewPipeline::Stop() {
  running_ = false;
}

} // namespace media