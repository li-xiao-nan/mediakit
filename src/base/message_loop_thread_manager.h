#ifndef BASE_MESSAGE_LOOP_THREAD_MANAGER_H
#define BASE_MESSAGE_LOOP_THREAD_MANAGER_H

#include "base/message_loop_thread.h"
#include "base/macros.h"

namespace media {
void PostTask(MKThreadId tid, AsyncTask async_task);

#define PostTaskDebug(tid, task) std::string task_from = std::string(__FILE__) + __FUNCTION__; \
   media::MessageLoopManager::GetInstance()->PostTask(tid, task, true, task_from);
#define POSTTASK(tid, task)                                     \
  std::string task_from = std::string(__FILE__) + __FUNCTION__; \
  media::MessageLoopManager::GetInstance()->PostTask(tid, task, false, task_from);

class MessageLoopManager {
 public:
  static MessageLoopManager* GetInstance();
  void PostTask(MKThreadId tid, AsyncTask async_task,
    bool out_task_run_statistics_info = false,
    const std::string& post_from="");
  void RunMainThreadTask();
  void StopAll();
 private:
  bool running_;
  MessageLoopManager();
  std::map<MKThreadId, std::unique_ptr<MessageLoop>> s_message_loop_instance_;
  DISALLOW_COPY_AND_ASSIGN(MessageLoopManager);
};
} // namespace media

#endif