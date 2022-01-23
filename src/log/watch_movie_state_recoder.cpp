#include "log/watch_movie_state_recoder.h"
#include "log/log_wrapper.h"
#include <fstream>
#include <sstream>
#include <boost/filesystem.hpp>

namespace media {
const char* kRecordeFileName = "watch_movie_record.json";
const char* kRecordeFileDir = "/config/";

WatchMovieStateRecoder::WatchMovieStateRecoder() : initialized(false){}

void WatchMovieStateRecoder::Initialize() {
  std::string app_dir = GetApplicationFileDirUtf8();
  record_info_file_path_ = app_dir + kRecordeFileDir + kRecordeFileName;
  
  if(!boost::filesystem::exists(record_info_file_path_)) {
    LogMessage(LOG_LEVEL_ERROR, "Recode info file isn't exist, path:" + record_info_file_path_);
    return;
  }

  Json::CharReaderBuilder reader;
  std::ifstream ifs_recoder_file;
  std::string error_content;
  ifs_recoder_file.open(record_info_file_path_, std::ios::binary, _SH_DENYNO);
  bool extract_result = Json::parseFromStream(reader, ifs_recoder_file,
    &record_info_root_, &error_content);
  ifs_recoder_file.close();
  
  if(!extract_result) {
    LogMessage(LOG_LEVEL_ERROR, 
      "read&parser film view recode info failed; error info:" + error_content);
    return;
  }
  initialized = true;
  return;
}

int64_t WatchMovieStateRecoder::GetFilmViewProgress(
  const std::string& film_name_utf8_format) {
  int64_t pre_view_timestamp = 0;
  if(!record_info_root_.isMember(film_name_utf8_format)) {
    return pre_view_timestamp;
  }
  
  pre_view_timestamp = record_info_root_[film_name_utf8_format].asInt64();

  return pre_view_timestamp;
}

void WatchMovieStateRecoder::RecordFilmViewProgress(
  const std::string& file_name_utf8_format, int64_t timestamp) {
  record_info_root_[file_name_utf8_format] = timestamp;
}

void WatchMovieStateRecoder::WriteToLocalFile() {
  std::ofstream ofs_record_file;
  //Json::StyledStreamWriter style_stream_writer;
  ofs_record_file.open(record_info_file_path_);
  //style_stream_writer.write(ofs_record_file, record_info_root_);
  Json::StreamWriterBuilder stream_builder;
  stream_builder.settings_["emitUTF8"] = true;
  std::unique_ptr<Json::StreamWriter> style_stream_writer(stream_builder.newStreamWriter());
  style_stream_writer->write(record_info_root_, &ofs_record_file);
  ofs_record_file.close();
}
} // namespace media