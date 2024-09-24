project "ImaGE-Core"
   kind "StaticLib"
   language "C++"
   cppdialect "C++20"
   targetdir "Binaries/%{cfg.buildcfg}"
   staticruntime "off"

   defines {
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
    "_SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING",
    "_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS",
    }

   files { "source/**.h", "source/**.cpp" }

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

   links {
    "opengl32.lib",
    "glfw3dll.lib",
    "glfw3.lib",
    "assimp-vc143-mt.lib",
    "Jolt.lib",
    "rttr_core_d.lib"
   }

   libdirs 
   {
      "../Libraries/**"
   }

   pchheader "pch.h"
   pchsource "source/pch.cpp"

   targetdir ("../Binaries/" .. OutputDir .. "/%{prj.name}")
   objdir ("../Binaries/Intermediates/" .. OutputDir .. "/%{prj.name}")

   filter "system:windows"
       systemversion "latest"
       defines { }

   filter "configurations:Debug"
       defines { "DEBUG" }
       runtime "Debug"
       symbols "On"

   filter "configurations:Release"
       defines { "RELEASE" }
       runtime "Release"
       optimize "On"
       symbols "On"

   filter "files:**.c"
       flags {"NoPCH"}

   filter "files:source/External/**.cpp"
       flags {"NoPCH"}
    
   filter "files:source/External/**.c"
       flags {"NoPCH"}

   postbuildcommands {
        '{COPYFILE} "%{prj.location}lib/glfw-3.3.8.bin.WIN64/lib-vc2022/glfw3.dll" "%{cfg.buildtarget.directory}"',
        '{COPYFILE} "%{prj.location}lib/assimp/assimp-vc143-mt.dll" "%{cfg.buildtarget.directory}"',
        '{COPYFILE} "%{prj.location}imgui.ini" "%{cfg.buildtarget.directory}"',
        '{MKDIR} "%{cfg.buildtarget.directory}/Shaders"',
        '{COPYDIR} "%{prj.location}Shaders" "%{cfg.buildtarget.directory}/Shaders"',
        '{MKDIR} "%{cfg.buildtarget.directory}/assets"',
        '{COPYDIR} "%{prj.location}assets" "%{cfg.buildtarget.directory}/assets"'
    }