#pragma once
#include <string>
#include <vector>
#include <rttr/variant.h>

namespace Reflection {

  template<typename T>
  struct ScriptFieldInfoProxy {
    std::string fieldName;
    std::string type;
    T data;
  };

  struct ProxyScript {
    std::string scriptName;
    std::vector<rttr::variant> scriptFieldProxyList;
  };

} // namespace Reflection
