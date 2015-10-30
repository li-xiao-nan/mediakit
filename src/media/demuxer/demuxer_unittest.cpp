#include "boost/smart_ptr/scoped_ptr.hpp"
#include "boost/asio/io_service.hpp"
#include "boost/function.hpp"

#include "ffmpeg_demuxer.h"
#include "net/url.h"
#include "net/io_channel.h"

void PipelineStatusCallBack(media::PipelineStatus status);
void SeekCB(media::PipelineStatus);

boost::asio::io_service* task_runner;
media::Demuxer* demuxer = NULL;

void StartDemux() {
  int64_t start_time = demuxer->GetStartTime();
  int64_t duration = demuxer->GetDuration();
  printf("************Media Infomation**************\n");
  printf("StartTime:%lld\n duration:%lld\n", start_time, duration);
  demuxer->ShowMediaConfigInfo();
  demuxer->Seek(demuxer->GetStartTime(), &SeekCB);
}

void SeekCB(media::PipelineStatus status) {
  switch (status) {
    case media::PIPELINE_ERROR_SEEK_FAILED:
      std::cout << "seek failed" << std::endl;
      break;
    case media::PIPELINE_OK:
      std::cout << "seek success" << std::endl;
      break;
  }
}

void PipelineStatusCallBack(media::PipelineStatus status) {
  switch (status) {
    case media::PIPELINE_OK:
      std::cout << "pipeline_ok" << std::endl;
      StartDemux();
      break;
    case media::PIPELINE_ERROR_SEEK_FAILED:
      std::cout << "occur error: PIPELINE_ERROR_SEEK_FAILED" << std::endl;
      break;
    case media::DEMUXER_OPEN_CONTEXT_FAILED:
      std::cout << "occur error: DEMUXER_OPEN_CONTEXT_FAILED" << std::endl;
      break;
    case media::DEMUXER_FIND_STREAM_INFO_FAILED:
      std::cout << "occur error: DEMUXER_OPEN_CONTEXT_FAILED" << std::endl;
      break;
    default:
      std::cout << "unknown error" << std::endl;
  }
}

int main(int argc, char* argv[]) {
  // init task_runner
  task_runner = new boost::asio::io_service();
  boost::asio::io_service::work io_service_work(*task_runner);
  boost::thread thread(boost::bind(&boost::asio::io_service::run, task_runner));

  // init datasoruce
  std::string url = "/Users/linan/Downloads/1.mp4";
  net::Url newUrl(url);
  std::shared_ptr<net::IOChannel> data_source(
      net::IOChannel::CreateIOChannel(newUrl));

  // init demuxer
  demuxer = new media::FFmpegDemuxer(task_runner, data_source);
  demuxer->Initialize(&PipelineStatusCallBack);

  sleep(10000);
  return 0;
}