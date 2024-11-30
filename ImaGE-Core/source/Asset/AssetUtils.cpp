#include <pch.h>
#include <filesystem>
#include "AssetUtils.h"
namespace IGE {
	namespace Assets {
        namespace fs = std::filesystem;

        bool CopyFileToAssets(const std::string& sourcePath, const std::string& destinationPath) {
            if (IsDirectoriesEqual(sourcePath, destinationPath)) return true;
            try {
              // No files should be copied/created in the distribution build
#ifndef DISTRIBUTION
                fs::copy(sourcePath, destinationPath, fs::copy_options::overwrite_existing);
#endif
                return true;
            }
            catch (const std::filesystem::filesystem_error& e) {
#ifdef _DEBUG
                std::cerr << "Error copying file: " << e.what() << std::endl;
#endif
                return false;
            }
        }

        bool DeleteFileAssets(const std::string& filePath) {
            try {
                fs::remove(filePath);
                return true;
            }
            catch (const std::filesystem::filesystem_error& e) {
#ifdef _DEBUG
                std::cerr << "Error deleting file: " << e.what() << std::endl;
#endif
                return false;
            }
        }

        std::string GetFileName(const std::string& filePath) {
            return std::filesystem::path(filePath).stem().string();
        }
        std::string GetFileExtension(const std::string& filePath) {
            return std::filesystem::path(filePath).extension().string();
        }
        std::string GetFileNameWithExtension(const std::string& filePath)
        {
            //return std::filesystem::path(filePath).filename().string();
            return GetFileName(filePath) + GetFileExtension(filePath);
        }
        std::string GetDirectoryPath(const std::string& filePath) {
            size_t pos = filePath.find_last_of("/\\");
            if (pos != std::string::npos) {
                return filePath.substr(0, pos);
            }
            return ""; // If no separator is found, return an empty string
        }
        std::string GetAbsolutePath(const std::string& filePath) {
            return std::filesystem::absolute(filePath).string();
        }
        void CreateDirectoryIfNotExists(const std::string& directoryPath) {
            if (!fs::exists(directoryPath)) {
                // Directory doesn't exist, create it
                fs::create_directory(directoryPath);
            }
        }
        bool IsDirectoriesEqual(const std::string &fp1, const std::string &fp2) {
            fs::path path1 {fp1};
            fs::path path2 {fp2};

            try {
                if (fs::equivalent(path1, path2)) {
                    return true;
                }
                else {
                    return false;
                }
            }
            catch ([[maybe_unused]] const fs::filesystem_error& e) {
                return false;
            }
        }
        bool IsDirectoriesEqual(const fs::path& path1, const fs::path& path2) {

            try {
                if (fs::equivalent(path1, path2)) {
                    return true;
                }
                else {
                    return false;
                }
            }
            catch ([[maybe_unused]] const fs::filesystem_error& e) {
                return false;
            }
        }
        bool IsPathWithinDirectory(const std::string& fp, const std::string& dirPath)
        {
            fs::path filePath{fp};
            fs::path directoryPath { dirPath};
            try {
                // Get the canonical (absolute, normalized) paths
                fs::path fileCanonical = fs::canonical(filePath);
                fs::path dirCanonical = fs::canonical(directoryPath);

                // Check if the file's canonical path starts with the directory's canonical path
                return std::mismatch(dirCanonical.begin(), dirCanonical.end(), fileCanonical.begin()).first == dirCanonical.end();
            }
            catch (const fs::filesystem_error& e) {
#ifdef _DEBUG
                std::cerr << "Error: " << e.what() << std::endl;
#endif
                return false;
            }
        }
        bool IsValidFilePath(const std::string& path) {
            fs::path filePath(path);

            // Check if the path exists and is a regular file
            if (fs::exists(filePath) && fs::is_regular_file(filePath)) {
                return true; // The path exists and is a file
            }

            return false; // The path does not exist or is not a regular file
        }
	}
}