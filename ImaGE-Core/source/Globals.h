#pragma once

template <typename T>
inline constexpr T EPSILON{ static_cast<T>(1e-5) };
template <typename T>
inline constexpr T WINDOW_WIDTH{ static_cast<T>(1920) };
template <typename T>
inline constexpr T WINDOW_HEIGHT{ static_cast<T>(1600) };
template <typename T>
inline constexpr T UI_SCALING_FACTOR{ static_cast<T>(10) };

// should move to config file and retrieve from asset manager
constexpr char gRootDirectory[] = "y3-gam";
constexpr char gTempDirectory[] = ".temp\\";
constexpr char gBackupDirectory[] = ".backup\\";
constexpr char gAssetsDirectory[] = "..\\Assets\\";
constexpr char gEditorAssetsDirectory[] = "..\\Assets\\.Editor\\";
constexpr char gScenesDirectory[] = "..\\Assets\\Scenes\\";
constexpr char gPrefabsDirectory[] = "..\\Assets\\Prefabs\\";
constexpr char gMaterialDirectory[] = "..\\Assets\\Materials\\";
constexpr char gAnimationsDirectory[] = "..\\Assets\\Animations\\";
constexpr char gVideosDirectory[] = "..\\Assets\\Videos\\";
constexpr char gMeshOutputDir[] = "..\\Assets\\Models\\";

constexpr char gSceneFileExt[] = ".scn";
constexpr char gPrefabFileExt[] = ".pfb";
constexpr char gSupportedModelFormats[] = ".fbx.obj";
constexpr char gMeshFileExt[] = ".imsh";
constexpr char gFontFileExt[] = ".ttf";
constexpr char gSpriteFileExt[] = ".dds";
constexpr char gMaterialFileExt[] = ".mat";
constexpr char gAnimationFileExt[] = ".anim";
constexpr char gSupportedAudioFormats[] = ".mp3.wav";
constexpr char gSupportedVideoFormats[] = ".mp4.mpg.mkv.mov.webm";


// Editor
constexpr char gRobotoBoldFontPath[] = "..\\Assets\\.Editor\\.Fonts\\Roboto-Bold.ttf";
constexpr char gRobotoMediumFontPath[] = "..\\Assets\\.Editor\\.Fonts\\Roboto-Medium.ttf";
constexpr char gRobotoThinFontPath[] = "..\\Assets\\.Editor\\.Fonts\\Roboto-Thin.ttf";
constexpr char gMontserratSemiBoldFontPath[] = "..\\Assets\\.Editor\\.Fonts\\Montserrat-SemiBold.ttf";
constexpr char gMontserratLightFontPath[] = "..\\Assets\\.Editor\\.Fonts\\Montserrat-Light.ttf";
constexpr char gMontserratRegularFontPath[] = "..\\Assets\\.Editor\\.Fonts\\Montserrat-Regular.ttf";
constexpr char gIconsFontPath[] = "..\\Assets\\.Editor\\.Fonts\\fa-solid-900.ttf";
constexpr int gEditorDefaultTheme = 1;

inline bool gIsGamePaused{ false };

#define RINIT glDisable(GL_FRAMEBUFFER_SRGB);