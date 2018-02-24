#include "string_utils.h"

namespace utils {
  std::string StringUtils::ToString(int value){
    std::ostringstream stream;
    stream << value;
    return stream.str();
  }

  std::string ToString(int64_t value) {
    std::ostringstream stream;
    stream << value;
    return stream.str();
  }

  std::wstring ToWString(int value) {
    std::wostringstream wstream;
    wstream << value;
    return wstream.str();
  }

  std::wstring ToWstring(int64_t value) {
    std::wostringstream wstream;
    wstream << value;
    return wstream.str();
  }
}  // namespace utils