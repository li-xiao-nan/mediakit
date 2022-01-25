#include "base/message_loop_thread.h"

namespace media {
MessageLoop::MessageLoop(MKThreadId mk_thread_id) {
  mk_thread_id_ = mk_thread_id;
  task_runner_.reset(new boost::asio::io_service());
  io_service_work_.reset(
      new boost::asio::io_service::work(*task_runner_.get()));
  message_loop_thread_.reset(new boost::thread(
      boost::bind(&boost::asio::io_service::run, task_runner_.get())));
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

} //  namespace media