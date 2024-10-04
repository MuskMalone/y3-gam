#pragma once
#include <string>

namespace IGE {
	namespace Assets {
		const std::string cAssetDirectory{ "assets\\" };
		bool CopyFileToAssets(const std::string& sourcePath, const std::string& destinationPath);
		bool DeleteFileAssets(const std::string& filePath);
        std::string GetFileName(const std::string& filePath);
        std::string GetFileExtension(const std::string& filePath);
		std::string GetAbsolutePath(const std::string& filePath);
		void CreateDirectoryIfNotExists(const std::string& directoryPath);
		bool IsDirectoriesEqual(const std::string fp1, const std::string fp2);
	}
}