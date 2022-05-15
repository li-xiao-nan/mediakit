#include "base/message_loop_thread.h"

namespace media {
MessageLoop::MessageLoop(MKThreadId mk_thread_id) {
  mk_thread_id_ = mk_thread_id;
  task_runner_.reset(new boost::asio::io_service());
  io_service_work_.reset(
      new boost::asio::io_service::work(*task_runner_.get()));
  boost::function<void(void)> thread_main_func;
  if(mk_thread_id == TID_DEMUXER) {
    thread_main_func = boost::bind(&MessageLoop::DemuxerThreadMain, this);
  } else if(mk_thread_id == TID_DECODE) {
    thread_main_func = boost::bind(&MessageLoop::DecodeThreadMain, this);
  } else if(mk_thread_id == TID_WORK) {
    thread_main_func = boost::bind(&MessageLoop::DecodeThreadMain, this);
  } else {
    BOOST_ASSERT_MSG(0, "unsupported thread id");
  }
  message_loop_thread_.reset(new boost::thread(thread_main_func));
}
unsigned int MessageLoop::GetTID() {
  return thread_id_;
}

MKThreadId MessageLoop::GetThreadType() {
  return mk_thread_id_;
}

void MessageLoop::PostTask(AsyncTask task) {
  task_runner_->post(task);
}

void MessageLoop::Stop() {
  io_service_work_.release();
  task_runner_->stop();
}

void MessageLoop::DemuxerThreadMain() {
  task_runner_->run();
}

void MessageLoop::DecodeThreadMain() {
  task_runner_->run();
}

void MessageLoop::WorkThreadMain() {
  task_runner_->run();
}
} //  namespace media