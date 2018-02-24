#include <iostream>
#include <sstream>
#include <iosfwd>
#include <string>
#include <stdint.h>

namespace utils{
  class StringUtils {
  public:
    static std::string ToString(int value);
    static std::string ToString(int64_t value);
    static std::wstring ToWString(int value);
    static std::wstring ToWstring(int64_t value);
  private:
    // forbidden to instance
    StringUtils(){}
  };

}  // namespace untils
