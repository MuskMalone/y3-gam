#include <pch.h>
#ifndef IMGUI_DISABLE
#include "AssetHelpers.h"
#include <fstream>

// windows api
#include <Windows.h>
#include <commdlg.h>	// to open file explorer
#include <shellapi.h>
#include <shlobj_core.h>  // enable selection in file explorer

namespace AssetHelpers
{

  std::string GetRelativeFilePath(std::string const& filepath, std::string const& rootDir) {
    return "." + filepath.substr(filepath.find(rootDir) + rootDir.size());
  }

  std::vector<std::string> SelectFilesFromExplorer(const char* winTitle, const char* extensionsFilter,
    unsigned numFilters, const char* initialDir)
  {
    OPENFILENAMEA fileName{};
    CHAR size[MAX_PATH * 10]{};

    ZeroMemory(&fileName, sizeof(fileName));
    fileName.lStructSize = sizeof(fileName);
    fileName.hwndOwner = NULL;
    fileName.lpstrFile = size;
    fileName.nMaxFile = sizeof(size);
    fileName.lpstrFilter = extensionsFilter;
    fileName.nFilterIndex = numFilters;			// number of filters
    fileName.lpstrFileTitle = NULL;
    fileName.nMaxFileTitle = 0;
    fileName.lpstrInitialDir = initialDir;	// initial directory
    fileName.lpstrTitle = winTitle;   // window title
    fileName.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR | OFN_ALLOWMULTISELECT | OFN_EXPLORER;

    if (GetOpenFileNameA(&fileName)) {
      std::vector<std::string> files{};
      char const* ptr{ fileName.lpstrFile };

      std::string const dir{ ptr + std::string("\\") };
      ptr += strlen(ptr) + 1;
      while (*ptr) {
        files.emplace_back(dir + ptr);
        ptr += strlen(ptr) + 1;
      }

      return files;
    }

    return {};
  }

  std::string LoadFileFromExplorer(const char* extensionsFilter, unsigned numFilters, const char* initialDir)
  {
    OPENFILENAMEA fileName{};
    CHAR size[MAX_PATH]{};

    ZeroMemory(&fileName, sizeof(fileName));
    fileName.lStructSize = sizeof(fileName);
    fileName.hwndOwner = NULL;
    fileName.lpstrFile = size;
    fileName.nMaxFile = sizeof(size);
    fileName.lpstrFilter = extensionsFilter;
    fileName.nFilterIndex = numFilters;			// number of filters
    fileName.lpstrFileTitle = NULL;
    fileName.nMaxFileTitle = 0;
    fileName.lpstrInitialDir = initialDir;	// initial directory
    fileName.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

    if (GetOpenFileNameA(&fileName)) {
      return GetRelativeFilePath(fileName.lpstrFile);
    }

    return {};
  }

  std::string SaveFileToExplorer(const char* extensionsFilter, unsigned numFilters, const char* initialDir)
  {
    OPENFILENAMEA fileName{};
    CHAR size[MAX_PATH]{};

    ZeroMemory(&fileName, sizeof(fileName));
    fileName.lStructSize = sizeof(fileName);
    fileName.hwndOwner = NULL;
    fileName.lpstrFile = size;
    fileName.nMaxFile = sizeof(size);
    fileName.lpstrFilter = extensionsFilter;
    fileName.nFilterIndex = numFilters;			// number of filters
    fileName.lpstrFileTitle = NULL;
    fileName.nMaxFileTitle = 0;
    fileName.lpstrInitialDir = initialDir;	// initial directory
    // OFN_NOCHANGEDIR specifies not to change the working dir
    fileName.Flags = OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;

    if (GetSaveFileNameA(&fileName)) {
      std::string const newFile{ GetRelativeFilePath(fileName.lpstrFile) };
      std::ofstream ofs{ newFile };
      if (ofs) {
        ofs.close();
        return newFile;
      }
    }

    // @TODO: replace with logger
    throw std::runtime_error("Unable to save file");
  }

  void OpenFileWithDefaultProgram(std::filesystem::path const& path)
  {
    std::wstring const absolutePath{ std::filesystem::absolute(path).wstring() };
    ShellExecute(NULL, NULL, absolutePath.c_str(), NULL, NULL, SW_SHOW);
  }

  void OpenFileInExplorer(std::filesystem::path const& filePath)
  {
    //ShellExecuteA(NULL, "open", mCurrentDir.string().c_str(), NULL, NULL, SW_SHOWDEFAULT);
    std::wstring const absolutePath{ std::filesystem::absolute(filePath).wstring() };
    PIDLIST_ABSOLUTE pidl = ILCreateFromPath(absolutePath.c_str());
    if (pidl) {
      SHOpenFolderAndSelectItems(pidl, 0, 0, 0);
      ILFree(pidl);
    }
    else {
      // log error
    }
  }

  void OpenDirectoryInExplorer(std::filesystem::path const& filePath)
  {
    std::string const absolutePath{ std::filesystem::absolute(filePath).string() };
    ShellExecuteA(NULL, "open", absolutePath.c_str(), NULL, NULL, SW_SHOWDEFAULT);
  }

} // namespace AssetHelpers

#endif  // IMGUI_DISABLE
