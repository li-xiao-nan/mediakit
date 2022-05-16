#ifndef BASE_MESSAGE_LOOP_THREAD_H
#define BASE_MESSAGE_LOOP_THREAD_H
#include "boost/asio.hpp"
#include "boost/thread.hpp"

#include "base/macros.h"

namespace media {
enum MKThreadId {
  TID_MAIN,
  TID_DEMUXER,
  TID_DECODE,
  TID_WORK
};

using AsyncTask = boost::function<void(void)>;

void PostTask(MKThreadId tid, AsyncTask async_task);
class MessageLoop {
public:
  MessageLoop(MKThreadId mk_thread_id);
  unsigned int GetTID();
  MKThreadId GetThreadType();
  void PostTask(AsyncTask async_task, bool out_task_run_statistics_info,
    const std::string& post_from);
  void RunNextTask();
  void Stop();
private:
  void DemuxerThreadMain();
  void DecodeThreadMain();
  void WorkThreadMain();
  void OnTaskRunCompleted();
private:
  int thread_id_;
  int pending_task_count_;
  MKThreadId mk_thread_id_;
  std::unique_ptr<boost::asio::io_service> task_runner_;
  std::unique_ptr<boost::thread> message_loop_thread_;
  std::unique_ptr<boost::asio::io_service::work> io_service_work_;

  DISALLOW_COPY_AND_ASSIGN(MessageLoop);

};
} // namespace media

#endif