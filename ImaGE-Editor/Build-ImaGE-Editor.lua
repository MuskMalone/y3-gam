project "ImaGE-Editor"
   kind "ConsoleApp"
   language "C++"
   cppdialect "C++20"
   targetdir "Binaries/%{cfg.buildcfg}"
   staticruntime "off"

   buildoptions { "/bigobj" }
   flags { "MultiProcessorCompile" }
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

      "../ImaGE-Core/source/External/ImGui",
      "../ImaGE-Core/source/External/ImGuizmo",
      "../ImaGE-Core/source/External/ImTerm/",
      "../ImaGE-Core/source/External/ImNodes/",
      "../Libraries/spdlog/include",

      "../ImaGE-Core/source/External/glad/include",
      "../ImaGE-Core/source/External/fmod/include",
      "../ImaGE-Core/source/External/stb",
      "../Libraries/assimp/include/",
      "../Libraries/entt/single_include",
      "../Libraries/freetype/include/",
      "../Libraries/glfw/include",
      "../Libraries/glm",
      "../Libraries/rapidjson/include/",
      "../Libraries/rttr/src",
      "../Libraries/PhysX/physx/include",
      "../Libraries/PhysX/pxshared/include",
      "../ImaGE-Core/source/External/pl_mpeg",
      --"../Libraries/filewatch",
      --"../Libraries/mono/**",
      "../ImaGE-Core/source/External/filewatch",
      "../ImaGE-Core/source/External/mono/include",

      "../Libraries/rttr/Built-Libraries/Debug/src",
      "../Libraries/DirectXTex/DirectXTex"
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
         "_CRT_SECURE_NO_WARNINGS",
         "_SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING",
         "_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS",
         "GLM_ENABLE_EXPERIMENTAL",
         "PX_PHYSX_STATIC_LIB",
         "RTTR_DLL"
      }

       runtime "Debug"
       symbols "On"

       prebuildcommands {
         --"{COPYFILE} %[%{wks.location}Libraries/mono/lib/mono-2.0-sgen.dll] %[%{cfg.targetdir}]",
            "{COPYFILE} %[%{wks.location}ImaGE-Core/source/External/mono/lib/mono-2.0-sgen.dll] %[%{cfg.targetdir}]",
         }
       postbuildcommands {
         "{COPYFILE} %[%{wks.location}Libraries/PhysX/physx/bin/win.x86_64.vc142.md/debug/PhysXGpu_64.dll] %[%{cfg.targetdir}]",
         "{COPYFILE} %[%{wks.location}ImaGE-Core/source/External/fmod/lib/fmodL.dll] %[%{cfg.targetdir}]",
         "{COPYFILE} %[%{prj.location}imgui.ini] %[%{cfg.targetdir}]",
         --"{COPYDIR} %[%{wks.location}Libraries/mono/4.5] %[%{cfg.targetdir}/4.5]"
         "{COPYDIR} %[%{wks.location}ImaGE-Core/source/External/mono/4.5] %[%{cfg.targetdir}/4.5]",
         "{COPYFILE} %[%{wks.location}Libraries/rttr/Built-Libraries/Debug/bin/Debug/rttr_core_d.dll] %[%{cfg.targetdir}]",
      }

   filter "configurations:Release"
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
       runtime "Release"
       optimize "On"
       symbols "On"
       
       prebuildcommands {
         --"{COPYFILE} %[%{wks.location}Libraries/mono/lib/mono-2.0-sgen.dll] %[%{cfg.targetdir}]",
         "{COPYFILE} %[%{wks.location}ImaGE-Core/source/External/mono/lib/mono-2.0-sgen.dll] %[%{cfg.targetdir}]",
         }
       postbuildcommands {
         "{COPYFILE} %[%{wks.location}Libraries/PhysX/physx/bin/win.x86_64.vc142.md/release/PhysXGpu_64.dll] %[%{cfg.targetdir}]",
         "{COPYFILE} %[%{wks.location}ImaGE-Core/source/External/fmod/lib/fmod.dll] %[%{cfg.targetdir}]",
         "{COPYFILE} %[%{prj.location}imgui.ini] %[%{cfg.targetdir}]",
         --"{COPYDIR} %[%{wks.location}Libraries/mono/4.5] %[%{cfg.targetdir}/4.5]"
         "{COPYDIR} %[%{wks.location}ImaGE-Core/source/External/mono/4.5] %[%{cfg.targetdir}/4.5]",
         "{COPYFILE} %[%{wks.location}Libraries/rttr/Built-Libraries/Release/bin/Release/rttr_core.dll] %[%{cfg.targetdir}]",
      }

   filter "configurations:Distribution"
      defines {
         "NDEBUG",
         "DISTRIBUTION",
         "_CRT_SECURE_NO_WARNINGS",
         "_SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING",
         "_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS",
         "GLM_ENABLE_EXPERIMENTAL",
         "PX_PHYSX_STATIC_LIB",
         "RTTR_DLL"
      }
      kind "WindowedApp"
      runtime "Release"
      optimize "On"
      symbols "Off"
      prebuildcommands {
         --"{COPYFILE} %[%{wks.location}Libraries/mono/lib/mono-2.0-sgen.dll] %[%{cfg.targetdir}]",
         "{COPYFILE} %[%{wks.location}ImaGE-Core/source/External/mono/lib/mono-2.0-sgen.dll] %[%{cfg.targetdir}]",
      }
      postbuildcommands {
         "{COPYFILE} %[%{wks.location}Libraries/PhysX/physx/bin/win.x86_64.vc142.md/release/PhysXGpu_64.dll] %[%{cfg.targetdir}]",
         "{COPYFILE} %[%{wks.location}ImaGE-Core/source/External/fmod/lib/fmod.dll] %[%{cfg.targetdir}]",
         "{COPYFILE} %[%{prj.location}imgui.ini] %[%{cfg.targetdir}]",
         --"{COPYDIR} %[%{wks.location}Libraries/mono/4.5] %[%{cfg.targetdir}/4.5]"
         "{COPYDIR} %[%{wks.location}ImaGE-Core/source/External/mono/4.5] %[%{cfg.targetdir}/4.5]",
         "{COPYFILE} %[%{wks.location}Libraries/rttr/Built-Libraries/Release/bin/Release/rttr_core.dll] %[%{cfg.targetdir}]",
      }