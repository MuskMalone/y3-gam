#pragma once
#include <string>
#include <vector>
#include <rttr/variant.h>

namespace Reflection {

  // proxy script field info
  template<typename T>
  struct ProxySFInfo {
    std::string fieldName;
    std::string type;
    T data;
  };

  struct ProxyScript {
    std::string scriptName;
    std::vector<rttr::variant> scriptFieldProxyList;
  };

} // namespace Reflection
