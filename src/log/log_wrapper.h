#ifndef LOG_LOG_WRAPPER_H
#define LOG_LOG_WRAPPER_H
#include <string>
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
}


#endif // #ifndef LOG_LOG_WRAPPER_H