#ifndef BASE_UTIL_H
#define BASE_UTIL_H

namespace base {
template <typename... Args>
std::string string_format(const std::string& format, Args... args) {
  size_t size =
      1 + snprintf(nullptr, 0, format.c_str(), args...);  // Extra space for \0
  // unique_ptr<char[]> buf(new char[size]);
  char* bytes = new char[size];
  snprintf(bytes, size, format.c_str(), args...);
  std::string result = std::string(bytes);
  delete[] bytes;
  return result;
}
}

#endif