project "ImaGE-Core"
   kind "StaticLib"
   language "C++"
   cppdialect "C++20"
   targetdir "Binaries/%{cfg.buildcfg}"
   staticruntime "off"

   files { "source/**.h", "source/**.cpp", "source/**.c" }

   includedirs 
   {
      "source"
   }

   links
   {
      "ImaGE-Script",
   }

   externalincludedirs
   {
      "source/External",
      "source/External/ImGui",
      "source/External/glad/include",
      "source/External/fmod/include",
      "source/External/ImTerm/",

      "../Libraries/assimp/include/",
      "../Libraries/Built-Libraries/Release/Libraries/assimp/include",
      "../Libraries/entt/single_include",
      "../Libraries/freetype/include/",
      "../Libraries/glfw/include",
      "../Libraries/glm",
      "../Libraries/rapidjson/include/**",
      "../Libraries/rttr/**",
      "../Libraries/spdlog/include",
      "../Libraries/ImTerm/include/**",
      "../Libraries/PhysX/physx/include/**",
      "../Libraries/PhysX/physx/include",
      "../Libraries/PhysX/pxshared/include/**",
      "../Libraries/PhysX/pxshared/include",
      --"../Libraries/mono/**",
      --"../Libraries/filewatch",
      "../ImaGE-Core/source/External/filewatch",
      "../ImaGE-Core/source/External/mono/**",
      
      "../Libraries/rttr/Built-Libraries/Debug/Libraries/rttr/**",
      "../Libraries/DirectXTex/**",
   }

   pchheader "pch.h"
   pchsource "source/pch.cpp"

   filter "files:**.c"
        flags {"NoPCH"}

   filter "files:source/External/**.cpp"
        flags {"NoPCH"}

   filter "files:source/External/**.c"
        flags {"NoPCH"}

   targetdir ("../Binaries/" .. OutputDir .. "/%{prj.name}")
   objdir ("../Binaries/Intermediates/" .. OutputDir .. "/%{prj.name}")

   removefiles 
   { 
       "source/External/ImGui/backends/imgui_impl_vulkan.cpp",
       "source/External/ImGui/backends/imgui_impl_vulkan.h",
   }

   filter "system:windows"
       systemversion "latest"
       defines { }

   filter "configurations:Debug"
       runtime "Debug"
       symbols "On"

       defines {
        "_DEBUG",
        "_CONSOLE",
        "_CRT_SECURE_NO_WARNINGS",
        "_SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING",
        "_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS",
        "GLM_ENABLE_EXPERIMENTAL",
        "PX_PHYSX_STATIC_LIB",
        "RTTR_DLL"
        }

       links {
        "opengl32.lib",
        "glfw3.lib",

        "assimp-vc143-mtd.lib",
        "zlibstaticd.lib",

        "rttr_core_d.lib",

        "PhysX_static_64.lib", 
        "PhysXCharacterKinematic_static_64.lib", 
        "PhysXCommon_static_64.lib", 
        "PhysXCooking_static_64.lib", 
        "PhysXExtensions_static_64.lib", 
        "PhysXFoundation_static_64.lib", 
        "PhysXPvdSDK_static_64.lib", 
        "PhysXVehicle_static_64.lib", 

        "DirectXTex.lib",
        "fmodL_vc.lib",
        "mono-2.0-sgen.lib",
       }

       libdirs 
       {
           "../Libraries/glfw/build/src/Debug",
          "../Libraries/Built-Libraries/Debug/Libraries/**",
          --"../Libraries/mono/msvc/build/sgen/x64/**",
          "../Libraries/PhysX/physx/bin/win.x86_64.vc142.md/debug",
          "../Libraries/DirectXTex/DirectXTex/Bin/Desktop_2022_Win10/x64/Debug/",
          "source/External/fmod/lib",
          "source/External/mono/lib",
          "../Libraries/rttr/Built-Libraries/Debug/**"

       }

       postbuildcommands {
            "{MKDIR} %[%{wks.location}/Binaries/" .. OutputDir .. "/Assets]",
            "{COPYDIR} %[%{wks.location}/Assets] %[%{wks.location}/Binaries/" .. OutputDir .. "/Assets]"
        }

   filter "configurations:Release"
       runtime "Release"
       optimize "On"
       symbols "On"

       defines {
        "NDEBUG",
        "_CONSOLE",
        "_CRT_SECURE_NO_WARNINGS",
        "_SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING",
        "_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS",
        "GLM_ENABLE_EXPERIMENTAL",
        "PX_PHYSX_STATIC_LIB",
        "RTTR_DLL"
        }

        libdirs 
        {
           "../Libraries/glfw/build/src/Release",
           "../Libraries/Built-Libraries/Release/Libraries/**",
           --"../Libraries/mono/msvc/build/sgen/x64/**",
           "../Libraries/PhysX/physx/bin/win.x86_64.vc142.md/release",
           "../Libraries/DirectXTex/DirectXTex/Bin/Desktop_2022_Win10/x64/Release/",
           "source/External/fmod/lib",
           "source/External/mono/lib",
          "../Libraries/rttr/Built-Libraries/Release/**"

        }

       links {
        "opengl32.lib",
        "glfw3.lib",

        "assimp-vc143-mt.lib",
        "zlibstatic.lib",

        "rttr_core.lib",

        "PhysX_static_64.lib", 
        "PhysXCharacterKinematic_static_64.lib", 
        "PhysXCommon_static_64.lib", 
        "PhysXCooking_static_64.lib", 
        "PhysXExtensions_static_64.lib", 
        "PhysXFoundation_static_64.lib", 
        "PhysXPvdSDK_static_64.lib", 
        "PhysXVehicle_static_64.lib", 

        "DirectXTex.lib",
        "fmod_vc.lib",
        "mono-2.0-sgen.lib",
       }

    filter "configurations:Distribution"
       defines {
          "DISTRIBUTION",
          "_CRT_SECURE_NO_WARNINGS",
          "_SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING",
          "_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS",
          "GLM_ENABLE_EXPERIMENTAL",
          "PX_PHYSX_STATIC_LIB",
          "RTTR_DLL"
       }
       runtime "Release"
       optimize "On"
       symbols "Off"

       libdirs 
       {
          "../Libraries/glfw/build/src/Release",
          "../Libraries/Built-Libraries/Release/Libraries/**",
          --"../Libraries/mono/msvc/build/sgen/x64/**",
          "../Libraries/PhysX/physx/bin/win.x86_64.vc142.md/release",
          "../Libraries/DirectXTex/DirectXTex/Bin/Desktop_2022_Win10/x64/Release/",
          "source/External/fmod/lib",
          "source/External/mono/lib",
          "../Libraries/rttr/Built-Libraries/Release/**"
       }

       links {
        "opengl32.lib",
        "glfw3.lib",

        "assimp-vc143-mt.lib",
        "zlibstatic.lib",

        "rttr_core.lib",

        "PhysX_static_64.lib", 
        "PhysXCharacterKinematic_static_64.lib", 
        "PhysXCommon_static_64.lib", 
        "PhysXCooking_static_64.lib", 
        "PhysXExtensions_static_64.lib", 
        "PhysXFoundation_static_64.lib", 
        "PhysXPvdSDK_static_64.lib", 
        "PhysXVehicle_static_64.lib", 

        "DirectXTex.lib",
        "fmod_vc.lib",
        "mono-2.0-sgen.lib",
       }