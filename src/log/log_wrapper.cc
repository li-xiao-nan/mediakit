#include "log/log_wrapper.h"
#include "spdlog/spdlog.h"
#include "spdlog/async.h"
#include "spdlog/async_logger.h"
#include "spdlog/sinks/rotating_file_sink.h"

#include <windows.h>

#include "boost/assert.hpp"
#include <string>
#include <codecvt>
#include <mutex>
#include <boost/filesystem.hpp>

const char* kLogFileName = "mediakit.log";
const int kMaxLogFileSize = 1024 * 1024 * 5;  // 5M
const int kMaxLogFileCount = 5;
const char* kLoggerName = "snail_messenger";

namespace media {
std::mutex log_init_mutex;
std::wstring GetApplicationFileDirUtf16() {
  TCHAR szPath[MAX_PATH];
  ::GetModuleFileName(NULL, szPath, MAX_PATH);
  std::wstring path(szPath);
  std::wstring file_name;
  if (path.find_last_of(L'/') != std::wstring::npos) {
    file_name = path.substr(0, path.find_last_of(L'/'));
  } else if (path.find_last_of(L'\\') != std::wstring::npos) {
    file_name = path.substr(0, path.find_last_of(L'\\'));
  }
  return file_name;
}

std::string GetApplicationFileDirUtf8() {
  std::wstring file_dir_utf16 = GetApplicationFileDirUtf16();
  return UTF16toUTF8(file_dir_utf16);
}

std::string UTF16toUTF8(const std::wstring& str) {
  std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> convert;
  std::string utf8_string = convert.to_bytes(str);
  return utf8_string;
}

void InitializeLog(){
  std::unique_lock<std::mutex> lck(log_init_mutex);
  static bool initialized = false;
  if(initialized) {
    return;
  }
  spdlog::drop_all();
  std::string log_file_dir = GetApplicationFileDirUtf8() + "/log/";
  if(!boost::filesystem::is_directory(log_file_dir)) {
    bool result =  boost::filesystem::create_directory(log_file_dir);
    if(!result) {
      BOOST_ASSERT_MSG(0, "Create directory fiale");
      return;
    }
  }
  std::string log_file_path = log_file_dir + kLogFileName;
  auto message_logger = spdlog::rotating_logger_mt<spdlog::async_factory>(
    kLoggerName,
    log_file_path,
    kMaxLogFileSize,
    kMaxLogFileCount); 
  message_logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %v");
  message_logger->set_level(spdlog::level::trace);
  spdlog::register_logger(message_logger);
  initialized =true;
}

void LogMessage(LogLevel log_level, const std::string& message) {
  std::shared_ptr<spdlog::logger> logger = spdlog::get(kLoggerName);
  if(logger == nullptr) {
    InitializeLog();
    logger = spdlog::get(kLoggerName);
  }
  BOOST_ASSERT_MSG(logger, "log system initialize failed");
  switch(log_level) {
  case LOG_LEVEL_DEBUG:
     logger->debug(message);
    break;
  case LOG_LEVEL_ERROR:
     logger->error(message);
    break;
  case LOG_LEVEL_INFO:
     logger->info(message);
    break;
  case LOG_LEVEL_WRANING:
     logger->warn(message);
    break;
  default:
    BOOST_ASSERT_MSG(0, "unsupported log level");
  }
  logger->flush();
  return;
}

} // namespace media