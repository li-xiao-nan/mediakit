#include "base/message_loop_thread_manager.h"

namespace media {
  void PostTask(MKThreadId tid, AsyncTask async_task) {
    MessageLoopManager::GetInstance()->PostTask(tid, async_task);
  }

  void MessageLoopManager::PostTask(MKThreadId tid, AsyncTask async_task) {
    if(tid == TID_MAIN) {
      main_thread_io_service_.post(async_task);
    }else {
      s_message_loop_instance_[tid]->PostTask(async_task);
    }
  }

  MessageLoopManager* MessageLoopManager::GetInstance() {
    static MessageLoopManager g_instance;
    return &g_instance;
  }

  void MessageLoopManager::RunMainThreadTask() {
    main_thread_io_service_.run();
  }

  MessageLoopManager::MessageLoopManager() {
    s_message_loop_instance_[TID_DEMUXER] = 
      std::make_unique<MessageLoop>(TID_DEMUXER);
    s_message_loop_instance_[TID_DECODE] = 
      std::make_unique<MessageLoop>(TID_DECODE);
    s_message_loop_instance_[TID_WORK] = 
      std::make_unique<MessageLoop>(TID_WORK);
  }

  void MessageLoopManager::StopAll() {
    main_thread_io_service_.stop();
    for(auto& item : s_message_loop_instance_) {
      item.second->Stop();
    }
  }
} // namespace media