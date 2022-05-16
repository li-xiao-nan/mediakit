#include "base/message_loop_thread_manager.h"
#include "base/base_type.h"

namespace media {
  void PostTask(MKThreadId tid, AsyncTask async_task) {
    MessageLoopManager::GetInstance()->PostTask(tid, async_task);
  }

  void MessageLoopManager::PostTask(MKThreadId tid, AsyncTask async_task,
    bool out_task_run_statistics_info, const std::string& post_from) {
    if(!running_) {
      return;
    }
    s_message_loop_instance_[tid]->PostTask(async_task,
      out_task_run_statistics_info, post_from);
  }

  MessageLoopManager* MessageLoopManager::GetInstance() {
    static MessageLoopManager g_instance;
    return &g_instance;
  }

  void MessageLoopManager::RunMainThreadTask() {
    if(!running_) false;
    s_message_loop_instance_[TID_MAIN]->RunNextTask();
  }

  MessageLoopManager::MessageLoopManager(): running_(true) {
    s_message_loop_instance_[TID_DEMUXER] = 
      std::make_unique<MessageLoop>(TID_DEMUXER);
    s_message_loop_instance_[TID_DECODE] = 
      std::make_unique<MessageLoop>(TID_DECODE);
    s_message_loop_instance_[TID_WORK] = 
      std::make_unique<MessageLoop>(TID_WORK);
    s_message_loop_instance_[TID_MAIN] = 
      std::make_unique<MessageLoop>(TID_MAIN);
  }

  void MessageLoopManager::StopAll() {
    running_ = false;
    for(auto& item : s_message_loop_instance_) {
      item.second->Stop();
    }
  }
} // namespace media