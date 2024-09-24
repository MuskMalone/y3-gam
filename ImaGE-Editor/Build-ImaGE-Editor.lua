project "ImaGE-Editor"
   kind "ConsoleApp"
   language "C++"
   cppdialect "C++20"
   targetdir "Binaries/%{cfg.buildcfg}"
   staticruntime "off"

   files { "source/**.h", "source/**.cpp" }

   includedirs
   {
      "source/",

	  -- Include Core
	  "../ImaGE-Core/source/",
      "../ImaGE-Core/source/External/",
      "../ImaGE-Core/source/External/ImGui/",
      --"../ImaGE-Core/lib/",
      "./ImaGE-Core/lib/spdlog",
      "./ImaGE-Core/lib/entt/include",
      "./ImaGE-Core/lib/jolt/include",
      "./ImaGE-Core/lib/rttr-0.9.6/include",
      "./ImaGE-Core/lib/rapidjson-1.1.0/include",
      "./ImaGE-Core/lib/glm/include",
      "./ImaGE-Core/lib/assimp/include",
      "./ImaGE-Core/lib/glad-gl-4.6/include",
      "./ImaGE-Core/lib/glfw-3.3.8.bin.WIN64/include"
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