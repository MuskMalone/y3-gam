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
      "../Libraries/PhysX/pxshared/include/**",
      "../Libraries/mono/msvc/include/**",
      "../Libraries/Built-Libraries/Debug/Libraries/rttr/**"
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

       postbuildcommands {
         "{COPYFILE} %[%{wks.location}Libraries/physx4/debug/PhysX_64.dll] %[%{cfg.targetdir}]",
         "{COPYFILE} %[%{wks.location}Libraries/physx4/debug/PhysXCommon_64.dll] %[%{cfg.targetdir}]",
         "{COPYFILE} %[%{wks.location}Libraries/physx4/debug/PhysXCooking_64.dll] %[%{cfg.targetdir}]",
         "{COPYFILE} %[%{wks.location}Libraries/physx4/debug/PhysXFoundation_64.dll] %[%{cfg.targetdir}]",
         "{COPYFILE} %[%{wks.location}Libraries/physx4/debug/PhysXGpu_64.dll] %[%{cfg.targetdir}]",
         "{COPYFILE} %[%{wks.location}Libraries/fmod/lib/fmodL.dll] %[%{cfg.targetdir}]",
         "{COPYFILE} %[%{prj.location}imgui.ini] %[%{cfg.targetdir}]"
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

       postbuildcommands {
         "{COPYFILE} %[%{wks.location}Libraries/physx4/release/PhysX_64.dll] %[%{cfg.targetdir}]",
         "{COPYFILE} %[%{wks.location}Libraries/physx4/release/PhysXCommon_64.dll] %[%{cfg.targetdir}]",
         "{COPYFILE} %[%{wks.location}Libraries/physx4/release/PhysXCooking_64.dll] %[%{cfg.targetdir}]",
         "{COPYFILE} %[%{wks.location}Libraries/physx4/release/PhysXFoundation_64.dll] %[%{cfg.targetdir}]",
         "{COPYFILE} %[%{wks.location}Libraries/physx4/release/PhysXGpu_64.dll] %[%{cfg.targetdir}]",
         "{COPYFILE} %[%{wks.location}Libraries/fmod/lib/fmod.dll] %[%{cfg.targetdir}]",
         "{COPYFILE} %[%{prj.location}imgui.ini] %[%{cfg.targetdir}]"
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

      postbuildcommands {
         "{COPYFILE} %[%{wks.location}Libraries/physx4/release/PhysX_64.dll] %[%{cfg.targetdir}]",
         "{COPYFILE} %[%{wks.location}Libraries/physx4/release/PhysXCommon_64.dll] %[%{cfg.targetdir}]",
         "{COPYFILE} %[%{wks.location}Libraries/physx4/release/PhysXCooking_64.dll] %[%{cfg.targetdir}]",
         "{COPYFILE} %[%{wks.location}Libraries/physx4/release/PhysXFoundation_64.dll] %[%{cfg.targetdir}]",
         "{COPYFILE} %[%{wks.location}Libraries/physx4/release/PhysXGpu_64.dll] %[%{cfg.targetdir}]",
         "{COPYFILE} %[%{wks.location}Libraries/fmod/lib/fmod.dll] %[%{cfg.targetdir}]",
         "{COPYFILE} %[%{prj.location}imgui.ini] %[%{cfg.targetdir}]",
      }