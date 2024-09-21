#pragma once

template <typename T>
inline constexpr T EPSILON{ static_cast<T>(1e-5) };
template <typename T>
inline constexpr T WINDOW_WIDTH{ static_cast<T>(1920) };
template <typename T>
inline constexpr T WINDOW_HEIGHT{ static_cast<T>(1080) };

// should move to config file and retrieve from asset manager
constexpr char gTempDirectory[] = ".temp\\";
constexpr char gAssetsDirectory[] = ".\\Assets\\";
constexpr char gScenesDirectory[] = ".\\Assets\\Scenes\\";
constexpr char gPrefabsDirectory[] = ".\\Assets\\Prefabs\\";
constexpr char gSceneFileExt[] = ".scn";
constexpr char gPrefabFileExt[] = ".pfb";
constexpr char gIconsFontPath[] = ".\\Source\\External\\ImGui\\misc\\fonts\\fa-solid-900.ttf";
constexpr char gRobotoBoldFontPath[] = ".\\Source\\External\\ImGui\\misc\\fonts\\Roboto-Bold.ttf";
constexpr char gRobotoMediumFontPath[] = ".\\Source\\External\\ImGui\\misc\\fonts\\Roboto-Medium.ttf";
constexpr char gRobotoThinFontPath[] = ".\\Source\\External\\ImGui\\misc\\fonts\\Roboto-Thin.ttf";