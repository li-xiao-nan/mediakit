#ifndef LOG_LOG_WRAPPER_H
#define LOG_LOG_WRAPPER_H
#include <sstream>
#include <string>
#include <chrono>
namespace media {
  enum LogLevel {
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_WRANING,
  };

  void EnableAVPacketProcessTrace();
  void DisableAVPacketProcessTrace();
  bool IsAVPacketProcessTraceEnabled();
  void InitializeLog();
  std::wstring GetApplicationFileDirUtf16();
  std::string GetApplicationFileDirUtf8();
  std::string UTF16toUTF8(const std::wstring& str);
  std::string UTF16toANSI(const std::wstring& str);
  std::string AnsiToUtf8(const std::string& str);
  std::string GetMovieNameUtf8(const std::string& movie_file_path);
  
  void LogMessage(LogLevel log_level, const std::string& message);

  void ffmpeg_log_callback(void* avcl, int level, const char* fmt, va_list vl);

  class FormatLogMessage{
  public:
    FormatLogMessage(const char* file,
      const char* function_name, int line, LogLevel log_level);
    ~FormatLogMessage();
    std::ostream& stream();
  private:
    LogLevel log_level_;
    std::ostringstream format_out_string_stream_;
  };

#define TRACEPOINT media::FormatLogMessage(__FILE__, __FUNCTION__, __LINE__, media::LOG_LEVEL_DEBUG).stream()
#define LOGGING(severity) media::FormatLogMessage(__FILE__, __FUNCTION__, __LINE__, severity).stream()
#define TraceAVPacketProcess(pts)   !IsAVPacketProcessTraceEnabled() ? (void)0 : (media::FormatLogMessage(__FILE__, __FUNCTION__, __LINE__, LOG_LEVEL_DEBUG).stream() << "pts:" << pts);

  class ScopeTimeCount {
   public:
    ScopeTimeCount(const std::string func_name)
        : time_count_object_name_(func_name) {
      start_timestamp_ =
          std::chrono::time_point_cast<std::chrono::milliseconds>(
              std::chrono::system_clock::now())
              .time_since_epoch()
              .count();
    }
    ~ScopeTimeCount() {
      int64_t cast_time =
          std::chrono::time_point_cast<std::chrono::milliseconds>(
              std::chrono::system_clock::now())
              .time_since_epoch()
              .count() -
          start_timestamp_;
      LOGGING(LOG_LEVEL_DEBUG)
          << time_count_object_name_ << "cast: " << cast_time << " ms.";
    }

   private:
    std::string time_count_object_name_;
    int64_t start_timestamp_;
  };
  }


#endif // #ifndef LOG_LOG_WRAPPER_H