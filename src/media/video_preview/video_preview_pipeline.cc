#include "media/video_preview/video_preview_pipeline.h"
#include "base/message_loop_thread_manager.h"

namespace media {
VideoPreviewPipeline::VideoPreviewPipeline(const std::string& video_url)
  : initialized_(false)
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
  MessageLoopManager::GetInstance()->PostTask(TID_WORK, task);
}

// run on work thread
void VideoPreviewPipeline::GetKeyFrameInternal(int64_t timestamp_ms,
  int width, int hegiht) {
  if(!initialized_ || working_) {
    return;
  }

  working_ = true;
  demuxer_unit_->Seek(timestamp_ms);
  video_decoder_unit_->ClearBuffer();
  std::shared_ptr<VideoFrame> video_frame = nullptr;
  // 视频解码过程，不一定每次都能返回解码结果，初始阶段一般会依赖4-5个
  // EncodedAVframe数据，才能完整初次解码过程
  while(true) {
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

  if(!video_frame) {
    return;
  }

  if(delegate_) {
    AsyncTask task = boost::bind(
      &VideoPreviewPipelineDelegate::OnGetKeyVideoFrame,
      delegate_, timestamp_ms, video_frame);
    MessageLoopManager::GetInstance()->PostTask(TID_MAIN, task);
  }
  working_ = false;
}

} // namespace media