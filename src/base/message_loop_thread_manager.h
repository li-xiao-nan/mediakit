#ifndef BASE_MESSAGE_LOOP_THREAD_MANAGER_H
#define BASE_MESSAGE_LOOP_THREAD_MANAGER_H

#include "base/message_loop_thread.h"
#include "base/macros.h"

namespace media {
void PostTask(MKThreadId tid, AsyncTask async_task);
class MessageLoopManager {
 public:
  static MessageLoopManager* GetInstance();
  void PostTask(MKThreadId tid, AsyncTask async_task);
  void StopAll();
 private:
  MessageLoopManager();
  std::map<MKThreadId, std::unique_ptr<MessageLoop>> s_message_loop_instance_;

  DISALLOW_COPY_AND_ASSIGN(MessageLoopManager);
};
} // namespace media

#endif