#pragma once
#include <string>
#include <filesystem>
namespace IGE {
	namespace Assets {
		bool CopyFileToAssets(const std::string& sourcePath, const std::string& destinationPath);
		bool DeleteFileAssets(const std::string& filePath);
        std::string GetFileName(const std::string& filePath);
        std::string GetFileExtension(const std::string& filePath);
		std::string GetAbsolutePath(const std::string& filePath);
		void CreateDirectoryIfNotExists(const std::string& directoryPath);
		bool IsDirectoriesEqual(const std::string& fp1, const std::string& fp2);
		bool IsDirectoriesEqual(const std::filesystem::path& fp1, const std::filesystem::path& fp2);
		bool IsPathWithinDirectory(const std::string &fp, const std::string& dirPath);
	}
}