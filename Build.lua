-- premake5.lua
workspace "ImaGE"
   architecture "x64"
   configurations { "Debug", "Release", "Distribution" }
   startproject "ImaGE-Editor"

   -- Workspace-wide build options for MSVC
   filter "system:windows"
      buildoptions { "/EHsc", "/Zc:preprocessor", "/Zc:__cplusplus" }

OutputDir = "%{cfg.system}-%{cfg.architecture}/%{cfg.buildcfg}"

filter "action:vs*"
   linkoptions { "/ignore:4099" }
   linkoptions { "/ignore:4006" }
   linkoptions { "/ignore:4005" }

group "Core"
	include "ImaGE-Core/Build-ImaGE-Core.lua"
group ""

group "Editor"
   include "ImaGE-Editor/Build-ImaGE-Editor.lua"
group ""

group "Game"
   include "ImaGE-Game/Build-ImaGE-Game.lua"
group ""