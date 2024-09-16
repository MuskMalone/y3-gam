#pragma once
#include <filesystem>

namespace AssetHelpers
{
	/*!*********************************************************************
	\brief
		Takes an absolute filepath and returns the relative file path
		based on the root directory passed in
	\param filepath
		The absolute filepath
	\param rootDir
		The root directory that will be replaced with "./"
	\return
		The filepath relative to the given root directory
	************************************************************************/
	std::string GetRelativeFilePath(std::string const& filepath, std::string const& rootDir = "ImaGE");

	/*!*********************************************************************
	\brief
		Opens the file explorer and allows the user to select 1 or more files.
		The filepath relative to initialDir is returned.
	\param extensionsFilter
		The list of file extensions to limit the explorer to show
		Format:
			A null character-separated list of filter pairs where each
			element will consist of:
				1. Label of the file extension
				2. Semicolon separated list of file extensions accepted by the
						filter

		Example:
			"Scenes (*.scn)\0*.scn\0All Files (*.*)\0*.*"
	\param numFilters
		The number of filters (should correspond to extensionsFilter)
	\param initialDir
		The filepath of the initial directory the file explorer opens up at.
		Defaults to Assets.
	\return
		The vector of filepaths selected
	************************************************************************/
	std::vector<std::string> SelectFilesFromExplorer(const char* winTitle = "Open", const char* extensionsFilter = "All Files (*.*)\0*.*",
		unsigned numFilters = 1, const char* initialDir = "./Assets");

	/*!*********************************************************************
	\brief
		Opens the file explorer and allows the user to select a file. The
		filepath relative to initialDir is returned.
	\param extensionsFilter
		The list of file extensions to limit the explorer to show
		Format:
			A null character-separated list of filter pairs where each
			element will consist of:
				1. Label of the file extension
				2. Semicolon separated list of file extensions accepted by the
						filter

		Example:
			"Scenes (*.scn)\0*.scn\0All Files (*.*)\0*.*"
	\param numFilters
		The number of filters (should correspond to extensionsFilter)
	\param initialDir
		The filepath of the initial directory the file explorer opens up at.
		Defaults to Assets.
	\return
		The filepath of the file selected
	************************************************************************/
	std::string LoadFileFromExplorer(const char* extensionsFilter = "All Files (*.*)\0*.*",
		unsigned numFilters = 1, const char* initialDir = "./Assets");

	/*!*********************************************************************
		\brief
			Opens the file explorer and allows the user to create a file. The
			filepath relative to initialDir is returned.
		\param extensionsFilter
			The list of file extensions to limit the explorer to show
			Format:
				A null character-separated list of filter pairs where each
				element will consist of:
					1. Label of the file extension
					2. Semicolon separated list of file extensions accepted by the
						 filter

			Example:
				"Scenes (*.scn)\0*.scn\0All Files (*.*)\0*.*"
		\param numFilters
			The number of filters (should correspond to extensionsFilter)
		\param initialDir
			The filepath of the initial directory the file explorer opens up at.
			Defaults to Assets.
		\return
			The filepath of the file selected
		************************************************************************/
	static std::string SaveFileToExplorer(const char* extensionsFilter = "All Files (*.*)\0*.*",
		unsigned numFilters = 1, const char* initialDir = "./Assets");

	/*!*********************************************************************
	\brief
		Opens the given file with the default program
	\param filePath
		The path of the file
	************************************************************************/
	void OpenFileWithDefaultProgram(std::filesystem::path const& filePath);

	/*!*********************************************************************
	\brief
		Opens the file explorer with mSelectedAsset selected
	************************************************************************/
	void OpenFileInExplorer(std::filesystem::path const& filePath);

	/*!*********************************************************************
	\brief
		Opens the directory in the file explorer
	************************************************************************/
	void OpenDirectoryInExplorer(std::filesystem::path const& filePath);

} // namespace AssetHelpers
