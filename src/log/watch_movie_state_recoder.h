#ifndef LOG_FILE_VIEW_RECODER_H
#define LOG_FILE_VIEW_RECODER_H
//  持久化存储观影记录，每次打开同一影片，自动基于上次播放时间，继续播放
//  1. 字符编码格式为：utf8
//  2. 数据组织格式为：json
//  3. json数据格式如下：
//  {
//       "平津湖" : 10, 
//       "岛国晕动片" : 10,
//   }

#include <string>
#include "json/json.h"

namespace media {
class WatchMovieStateRecoder{
public:
  WatchMovieStateRecoder();
  void Initialize();
  int64_t GetFilmViewProgress(const std::string& film_name_utf8_format);
  void RecordFilmViewProgress(const std::string& file_name_utf8_format,
    int64_t timestamp);
  void WriteToLocalFile();
private:
  bool initialized;
  std::string record_info_file_path_;
  Json::Value record_info_root_;
};
} // namespace media
#endif //#ifndef LOG_FILE_VIEW_RECODER_H
