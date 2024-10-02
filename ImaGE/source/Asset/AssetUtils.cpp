#include <pch.h>
#include <filesystem>
#include "AssetUtils.h"
namespace IGE {
	namespace Assets {
        namespace fs = std::filesystem;

        bool CopyFileToAssets(const std::string& sourcePath, const std::string& destinationPath) {
            try {
                fs::copy(sourcePath, destinationPath, fs::copy_options::overwrite_existing);
                return true;
            }
            catch (const std::filesystem::filesystem_error& e) {
                std::cerr << "Error copying file: " << e.what() << std::endl;
                return false;
            }
        }

        bool DeleteFileAssets(const std::string& filePath) {
            try {
                fs::remove(filePath);
                return true;
            }
            catch (const std::filesystem::filesystem_error& e) {
                std::cerr << "Error deleting file: " << e.what() << std::endl;
                return false;
            }
        }

        std::string GetFileName(const std::string& filePath) {
            return std::filesystem::path(filePath).stem().string();
        }
        std::string GetFileExtension(const std::string& filePath) {
            return std::filesystem::path(filePath).extension().string();
        }
        std::string GetAbsolutePath(const std::string& filePath) {
            return std::filesystem::absolute(filePath).string();
        }
        void CreateDirectoryIfNotExists(const std::string& directoryPath) {
            if (!fs::exists(directoryPath)) {
                // Directory doesn't exist, create it
                if (fs::create_directory(directoryPath)) {
                    std::cout << "Directory created: " << directoryPath << std::endl;
                }
                else {
                    std::cerr << "Failed to create directory: " << directoryPath << std::endl;
                }
            }
            else {
                std::cout << "Directory already exists: " << directoryPath << std::endl;
            }
        }
	}
}