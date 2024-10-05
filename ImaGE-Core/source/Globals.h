#pragma once

template <typename T>
inline constexpr T EPSILON{ static_cast<T>(1e-5) };
template <typename T>
inline constexpr T WINDOW_WIDTH{ static_cast<T>(1920) };
template <typename T>
inline constexpr T WINDOW_HEIGHT{ static_cast<T>(1600) };

// should move to config file and retrieve from asset manager
constexpr char gTempDirectory[] = ".temp\\";
constexpr char gAssetsDirectory[] = "..\\Assets\\";
constexpr char gScenesDirectory[] = "..\\Assets\\Scenes\\";
constexpr char gPrefabsDirectory[] = "..\\Assets\\Prefabs\\";
constexpr char gSceneFileExt[] = ".scn";
constexpr char gPrefabFileExt[] = ".pfb";
constexpr char gSupportedModelFormats[] = ".fbx.obj";
constexpr char gMeshOutputDir[] = "..\\Assets\\Models\\";
constexpr char gMeshFileExt[] = ".imsh";

// Editor
constexpr char gRobotoBoldFontPath[] = "..\\Assets\\Editor-Fonts\\Roboto-Bold.ttf";
constexpr char gRobotoMediumFontPath[] = "..\\Assets\\Editor-Fonts\\Roboto-Medium.ttf";
constexpr char gRobotoThinFontPath[] = "..\\Assets\\Editor-Fonts\\Roboto-Thin.ttf";
constexpr char gMontserratSemiBoldFontPath[] = "..\\Assets\\Editor-Fonts\\Montserrat-SemiBold.ttf";
constexpr char gMontserratLightFontPath[] = "..\\Assets\\Editor-Fonts\\Montserrat-Light.ttf";
constexpr char gMontserratRegularFontPath[] = "..\\Assets\\Editor-Fonts\\Montserrat-Regular.ttf";
constexpr char gIconsFontPath[] = "..\\Assets\\Editor-Fonts\\fa-solid-900.ttf";
constexpr int gEditorDefaultTheme = 1;