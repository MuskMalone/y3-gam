project "ImaGE-Editor"
   kind "ConsoleApp"
   language "C++"
   cppdialect "C++20"
   targetdir "Binaries/%{cfg.buildcfg}"
   staticruntime "off"

   files { "source/**.h", "source/**.cpp", "source/**.c" }

   includedirs
   {
      "source",

	  -- Include Core
	  "../ImaGE-Core/source",
   }

   externalincludedirs
   {
      "../ImaGE-Core/source/External",
      "./ImaGE-Core/source/External/ImGui",
      "../Libraries/**",
   }

   links
   {
      "ImaGE-Core",
   }

   targetdir ("../Binaries/" .. OutputDir .. "/%{prj.name}")
   objdir ("../Binaries/Intermediates/" .. OutputDir .. "/%{prj.name}")

   filter "system:windows"
       systemversion "latest"
       defines { "WINDOWS" }

   filter "configurations:Debug"
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

       runtime "Debug"
       symbols "On"

       postbuildcommands {
         "{COPYFILE} %[%{wks.location}Libraries/glfw-3.3.8.bin.WIN64/lib-vc2022/glfw3.dll] %[%{cfg.targetdir}]",
         "{COPYFILE} %[%{wks.location}Libraries/assimp/assimp-vc143-mt.dll] %[%{cfg.targetdir}]",
         "{COPYFILE} %[%{wks.location}Libraries/rttr-0.9.6/lib/Debug/rttr_core_d.dll] %[%{cfg.targetdir}]",
         "{COPYFILE} %[%{prj.location}imgui.ini] %[%{cfg.targetdir}]",
         "{MKDIR} %[%{cfg.buildtarget.directory}/assets]",
         "{COPYDIR} %[%{prj.location}assets] %[%{cfg.targetdir}/assets]"
      }

   filter "configurations:Release"
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
       runtime "Release"
       optimize "On"
       symbols "On"

       postbuildcommands {
         "{COPYFILE} %[%{wks.location}Libraries/glfw-3.3.8.bin.WIN64/lib-vc2022/glfw3.dll] %[%{cfg.targetdir}]",
         "{COPYFILE} %[%{wks.location}Libraries/assimp/assimp-vc143-mt.dll] %[%{cfg.targetdir}]",
         "{COPYFILE} %[%{wks.location}Libraries/rttr-0.9.6/lib/Release/rttr_core.dll] %[%{cfg.targetdir}]",
         "{COPYFILE} %[%{prj.location}imgui.ini] %[%{cfg.targetdir}]",
         "{MKDIR} %[%{cfg.buildtarget.directory}/assets]",
         "{COPYDIR} %[%{prj.location}assets] %[%{cfg.targetdir}/assets]"
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

      postbuildcommands {
         "{COPYFILE} %[%{wks.location}Libraries/glfw-3.3.8.bin.WIN64/lib-vc2022/glfw3.dll] %[%{cfg.targetdir}]",
         "{COPYFILE} %[%{wks.location}Libraries/assimp/assimp-vc143-mt.dll] %[%{cfg.targetdir}]",
         "{COPYFILE} %[%{wks.location}Libraries/rttr-0.9.6/lib/Release/rttr_core.dll] %[%{cfg.targetdir}]",
         "{COPYFILE} %[%{prj.location}imgui.ini] %[%{cfg.targetdir}]",
         "{MKDIR} %[%{cfg.buildtarget.directory}/assets]",
         "{COPYDIR} %[%{prj.location}assets] %[%{cfg.targetdir}/assets]"
      }