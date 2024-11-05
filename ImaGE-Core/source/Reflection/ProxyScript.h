/*!*********************************************************************
\file   ProxyScripts.h
\author chengen.lau\@digipen.edu
\date   16-October-2024
\brief  Struct used to hold raw json data of scripts. It acts as an
        intermediary object to handle script components differently
        from the rest during deserialization. It will only be
        deserialized to create a script instance when the component
        is added.

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#pragma once
#include <string>
#include <vector>
#include <rttr/variant.h>

namespace Reflection {


  struct ProxyScript {
    ProxyScript() = default;
    ProxyScript(const char* name) : scriptName{ name }, scriptFieldProxyList{} {}

    std::string scriptName;
    std::vector<rttr::variant> scriptFieldProxyList;
  };

  struct ProxyScriptComponent {
    std::vector<ProxyScript> proxyScriptList;
  };

} // namespace Reflection
