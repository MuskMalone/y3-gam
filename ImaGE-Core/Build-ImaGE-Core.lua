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

   externalincludedirs
   {
      "source/External",
      "source/External/ImGui",
      "../Libraries/**",
   }

   libdirs 
   {
      "../Libraries/**"
   }

   pchheader "pch.h"
   pchsource "source/pch.cpp"

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
        "JPH_DEBUG_RENDERER",
        "JPH_PROFILE_ENABLED",
        "JPH_OBJECT_STREAM",
        "JPH_USE_AVX2",
        "JPH_USE_AVX",
        "JPH_USE_SSE4_1",
        "JPH_USE_SSE4_2",
        "JPH_USE_LZCNT",
        "JPH_USE_TZCNT",
        "JPH_USE_F16C",
        "JPH_USE_FMADD",
        "_CRT_SECURE_NO_WARNINGS",
        "_SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING",
        "_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS"
        }

       links {
        "opengl32.lib",
        "glfw3dll.lib",
        "glfw3.lib",
        "assimp-vc143-mt.lib",
        "Jolt_d.lib",
        "Jolt1_d.lib",
        "rttr_core_d.lib"
       }

   filter "configurations:Release"
       runtime "Release"
       optimize "On"
       symbols "On"

       defines {
        "NDEBUG",
        "_CONSOLE",
        "JPH_OBJECT_STREAM",
        "JPH_USE_AVX2",
        "JPH_USE_AVX",
        "JPH_USE_SSE4_1",
        "JPH_USE_SSE4_2",
        "JPH_USE_LZCNT",
        "JPH_USE_TZCNT",
        "JPH_USE_F16C",
        "JPH_USE_FMADD",
        "_CRT_SECURE_NO_WARNINGS",
        "_SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING",
        "_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS"
        }

       links {
        "opengl32.lib",
        "glfw3dll.lib",
        "glfw3.lib",
        "assimp-vc143-mt.lib",
        "Jolt.lib",
        "Jolt1.lib",
        "rttr_core.lib"
       }

    filter "configurations:Distribution"
       defines {
          "DISTRIBUTION",
          "JPH_OBJECT_STREAM",
          "JPH_USE_AVX2",
          "JPH_USE_AVX",
          "JPH_USE_SSE4_1",
          "JPH_USE_SSE4_2",
          "JPH_USE_LZCNT",
          "JPH_USE_TZCNT",
          "JPH_USE_F16C",
          "JPH_USE_FMADD",
          "_CRT_SECURE_NO_WARNINGS",
          "_SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING",
          "_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS"
       }
       runtime "Release"
       optimize "On"
       symbols "Off"

       links {
        "opengl32.lib",
        "glfw3dll.lib",
        "glfw3.lib",
        "assimp-vc143-mt.lib",
        "Jolt.lib",
        "Jolt1.lib",
        "rttr_core.lib"
       }

   filter "files:**.c"
       flags {"NoPCH"}

   filter "files:source/External/**.cpp"
       flags {"NoPCH"}
    
   filter "files:source/External/**.c"
       flags {"NoPCH"}