project "ImaGE-Script"
   kind "SharedLib"
   language "C#"
   targetdir "Binaries/%{cfg.buildcfg}"

   files { "source/**.cs" }

   framework "net48"

   links {
      "System",
      "System.Core",
      "System.Collections",
      "System.IO",
      "System.Xml",
      "System.Data",
      "System.Net.Http",
      "System.Threading",
      "System.Windows"
  }

   targetdir ("../Binaries/" .. OutputDir .. "/%{prj.name}")
   objdir ("../Binaries/Intermediates/" .. OutputDir .. "/%{prj.name}")

   filter "system:windows"
       systemversion "latest"
       defines { "WINDOWS" }

   filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"