#ifndef LOG_LOG_WRAPPER_H
#define LOG_LOG_WRAPPER_H
#include <string>
#include <chrono>
namespace media {
  enum LogLevel {
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_WRANING,
  };
  void InitializeLog();
  std::wstring GetApplicationFileDirUtf16();
  std::string GetApplicationFileDirUtf8();
  std::string UTF16toUTF8(const std::wstring& str);
  
  void LogMessage(LogLevel log_level, const std::string& message);
  class ScopeTimeCount {
  public:
    ScopeTimeCount(const std::string func_name):
      time_count_object_name_(func_name) {
      start_timestamp_ = std::chrono::time_point_cast<std::chrono::milliseconds>(
          std::chrono::system_clock::now()).time_since_epoch().count();
    }
    ~ScopeTimeCount() {
      int cast_time = std::chrono::time_point_cast<std::chrono::milliseconds>(
                          std::chrono::system_clock::now())
                          .time_since_epoch()
                          .count() - start_timestamp_;
      LogMessage(LOG_LEVEL_DEBUG,
        time_count_object_name_ + "cast:" + std::to_string(cast_time) + " ms.");
    }
  private:
    std::string time_count_object_name_;
    int start_timestamp_;
  };
}


#endif // #ifndef LOG_LOG_WRAPPER_H