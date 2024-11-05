project "ImaGE-Script"
   kind "SharedLib"
   language "C#"
   targetdir "Binaries/%{cfg.buildcfg}"

   files { "source/**.cs" }

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

   buildoptions { "/p:RuntimeIdentifiers=win-x64" }  

   filter "system:windows"
       systemversion "latest"
       defines { "WINDOWS" }

   filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"

      postbuildcommands {
         "{COPYFILE} %[ImaGE-Script.dll] %[%{wks.location}Assets/Scripts/] ",
         "{COPYFILE} %[ImaGE-Script.pdb] %[%{wks.location}Assets/Scripts/] ",
     }

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"