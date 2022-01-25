#ifndef BASE_MESSAGE_LOOP_THREAD_H
#define BASE_MESSAGE_LOOP_THREAD_H
#include "boost/asio.hpp"
#include "boost/thread.hpp"

namespace media {
enum MKThreadId {
  TID_DEMUXER,
  TID_DECODE,
};

using AsyncTask = boost::function<void(void)>;

void PostTask(MKThreadId tid, AsyncTask async_task);
class MessageLoop {
public:
  MessageLoop(MKThreadId mk_thread_id);
  unsigned int GetTID();
  MKThreadId GetThreadType();
  void PostTask(AsyncTask task);
private:
private:
  int thread_id_;
  MKThreadId mk_thread_id_;
  std::unique_ptr<boost::asio::io_service> task_runner_;
  std::unique_ptr<boost::thread> message_loop_thread_;
  std::unique_ptr<boost::asio::io_service::work> io_service_work_;

};
} // namespace media

#endif