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
    "IMGUI_DISABLE"
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
      "../Libraries/**"
      --"../Libraries/spdlog/**",
      --"../Libraries/entt/include/**",
      --"../Libraries/jolt/include/**",
      --"../Libraries/rttr-0.9.6/include/**",
      --"../Libraries/rapidjson-1.1.0/include/**",
      --"../Libraries/glm/include/**",
      --"../Libraries/assimp/includ/**",
      --"../Libraries/glad-gl-4.6/include/**",
      --"../Libraries/glfw-3.3.8.bin.WIN64/include/**"
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

    postbuildcommands {
        '{COPY} "$(ProjectDir)lib/glfw-3.3.8.bin.WIN64/lib-vc2022/glfw3.dll" "$(OutDir)"',
        '{COPY} "$(ProjectDir)lib/assimp/assimp-vc143-mt.dll" "$(OutDir)"',
        '{COPY} "$(ProjectDir)imgui.ini" "$(OutDir)"',
        '{MKDIR} "$(OutDir)Shaders"',
        '{COPYDIR} "$(ProjectDir)Shaders" "$(OutDir)Shaders"',
        '{MKDIR} "$(OutDir)assets"',
        '{COPYDIR} "$(ProjectDir)assets" "$(OutDir)assets"'
    }