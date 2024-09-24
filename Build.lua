-- premake5.lua
workspace "ImaGE"
   architecture "x64"
   configurations { "Debug", "Release" }
   startproject "ImaGE-Editor"

   -- Workspace-wide build options for MSVC
   filter "system:windows"
      buildoptions { "/EHsc", "/Zc:preprocessor", "/Zc:__cplusplus" }

OutputDir = "%{cfg.system}-%{cfg.architecture}/%{cfg.buildcfg}"

group "Core"
	include "ImaGE-Core/Build-ImaGE-Core.lua"
group ""

group "Editor"
   include "ImaGE-Editor/Build-ImaGE-Editor.lua"
group ""