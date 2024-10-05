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
      "../ImaGE-Core/source/External/ImGui",
      "../ImaGE-Core/source/External/glad/include",
      "../ImaGE-Core/source/External/fmod/include",
      "../ImaGE-Core/source/External/ImTerm/",
      
      "../Libraries/assimp/include/**",
      "../Libraries/entt/single_include",
      "../Libraries/freetype/include/**",
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
      "../Libraries/filewatch",
      "../Libraries/mono/**",
      "../Libraries/Built-Libraries/Debug/Libraries/rttr/**",
      "../Libraries/DirectXTex/**"
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
      }

       runtime "Debug"
       symbols "On"

       prebuildcommands {
         "{COPYFILE} %[%{wks.location}Libraries/mono/lib/mono-2.0-sgen.dll] %[%{cfg.targetdir}]",

         }
       postbuildcommands {
         "{COPYFILE} %[%{wks.location}Libraries/PhysX/physx/bin/win.x86_64.vc142.md/debug/PhysXGpu_64.dll] %[%{cfg.targetdir}]",
         "{COPYFILE} %[%{wks.location}ImaGE-Core/source/Externals/fmod/lib/fmodL.dll] %[%{cfg.targetdir}]",
         "{COPYFILE} %[%{prj.location}imgui.ini] %[%{cfg.targetdir}]",
         "{COPYDIR} %[%{wks.location}Libraries/mono/4.5] %[%{cfg.targetdir}/4.5]"
      }

   filter "configurations:Release"
      defines {
         "NDEBUG",
         "_CONSOLE",
         "_CRT_SECURE_NO_WARNINGS",
         "_SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING",
         "_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS",
         "GLM_ENABLE_EXPERIMENTAL",
      }
       runtime "Release"
       optimize "On"
       symbols "On"
       prebuildcommands {
         "{COPYFILE} %[%{wks.location}Libraries/mono/lib/mono-2.0-sgen.dll] %[%{cfg.targetdir}]",

         }
       postbuildcommands {
         "{COPYFILE} %[%{wks.location}Libraries/PhysX/physx/bin/win.x86_64.vc142.md/release/PhysXGpu_64.dll] %[%{cfg.targetdir}]",
         "{COPYFILE} %[%{wks.location}ImaGE-Core/source/Externals/fmod/lib/fmod.dll] %[%{cfg.targetdir}]",
         "{COPYFILE} %[%{prj.location}imgui.ini] %[%{cfg.targetdir}]",
         "{COPYDIR} %[%{wks.location}Libraries/mono/4.5] %[%{cfg.targetdir}/4.5]"
      }

   filter "configurations:Distribution"
      defines {
         "DISTRIBUTION",
         "_CRT_SECURE_NO_WARNINGS",
         "_SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING",
         "_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS",
         "GLM_ENABLE_EXPERIMENTAL",
      }
      kind "WindowedApp"
      runtime "Release"
      optimize "On"
      symbols "Off"
      prebuildcommands {
         "{COPYFILE} %[%{wks.location}Libraries/mono/lib/mono-2.0-sgen.dll] %[%{cfg.targetdir}]",

         }
      postbuildcommands {
         "{COPYFILE} %[%{wks.location}Libraries/PhysX/physx/bin/win.x86_64.vc142.md/release/PhysXGpu_64.dll] %[%{cfg.targetdir}]",
         "{COPYFILE} %[%{wks.location}ImaGE-Core/source/Externals/fmod/lib/fmod.dll] %[%{cfg.targetdir}]",
         "{COPYFILE} %[%{prj.location}imgui.ini] %[%{cfg.targetdir}]",
         "{COPYDIR} %[%{wks.location}Libraries/mono/4.5] %[%{cfg.targetdir}/4.5]"
      }