#pragma once

template <typename T>
inline constexpr T EPSILON{ static_cast<T>(1e-5) };
template <typename T>
inline constexpr T WINDOW_WIDTH{ static_cast<T>(1600) };
template <typename T>
inline constexpr T WINDOW_HEIGHT{ static_cast<T>(900) };

// should move to config file and retrieve from asset manager
constexpr char gTempDirectory[] = ".temp\\";
constexpr char gAssetsDirectory[] = ".\\Assets\\";
