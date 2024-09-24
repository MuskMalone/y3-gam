project "ImaGE-Editor"
   kind "ConsoleApp"
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
      "source/**",

	  -- Include Core
	  "../ImaGE-Core/source/**",
   }

   externalincludedirs
   {
      "../ImaGE-Core/source/External/**",
      "../Libraries/spdlog/**",
      "../Libraries/entt/include/**",
      "../Libraries/jolt/include/**",
      "../Libraries/rttr-0.9.6/include/**",
      "../Libraries/rapidjson-1.1.0/include/**",
      "../Libraries/glm/include/**",
      "../Libraries/assimp/include/**",
      "../Libraries/glad-gl-4.6/include/**",
      "../Libraries/glfw-3.3.8.bin.WIN64/include/**"
   }

   links
   {
      "ImaGE-Core"
   }

   targetdir ("../Binaries/" .. OutputDir .. "/%{prj.name}")
   objdir ("../Binaries/Intermediates/" .. OutputDir .. "/%{prj.name}")

   filter "system:windows"
       systemversion "latest"
       defines { "WINDOWS" }

   filter "configurations:Debug"
       defines { "DEBUG" }
       runtime "Debug"
       symbols "On"

   filter "configurations:Release"
       defines { "RELEASE" }
       runtime "Release"
       optimize "On"
       symbols "On"