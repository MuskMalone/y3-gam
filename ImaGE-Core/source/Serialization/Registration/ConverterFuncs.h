#include <string>
#include <rttr/type.h>

namespace RTTRHelper {
  std::string StringFromRttrType(rttr::type const& type, bool& ok) {
    ok = true; return type.get_name().to_string();
  }

  rttr::type RttrTypeFromString(std::string const& str, bool& ok) {
    ok = true; return rttr::type::get_by_name(str);
  }
}
