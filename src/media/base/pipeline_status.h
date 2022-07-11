//
//  Header.h
//  mediacore
//
//  Created by 李楠 on 15/9/9.
//
//

#ifndef MEDIA_BASE_PIPELINE_STATUS_H_
#define MEDIA_BASE_PIPELINE_STATUS_H_
#include "boost/function.hpp"
namespace media {

enum PipelineStatus {
  PIPELINE_OK,
  PIPELINE_ERROR_SEEK_FAILED,
  DEMUXER_OPEN_CONTEXT_FAILED,
  DEMUXER_FIND_STREAM_INFO_FAILED,
  AUDIO_RENDERER_INIT_FAILED,
  VIDEO_RENDERER_INIT_FAILED,
};

typedef boost::function<void(PipelineStatus statues)> PipelineStatusCB;

}  // namespace media
#endif
