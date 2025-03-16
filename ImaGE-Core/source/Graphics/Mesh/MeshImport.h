#pragma once
// import strings
#define IMSH_IMPORT_RECENTER    "recenterMesh"
#define IMSH_IMPORT_NORM_SCALE  "normalizedScale"
#define IMSH_IMPORT_FLIP_UV     "flipUVs"
#define IMSH_IMPORT_STATIC      "staticMesh"
#define IMSH_IMPORT_MIN_FLAGS   "minimalFlags"

namespace Graphics::AssetIO
{
  struct ImportSettings {
    bool staticMesh = false;
    bool recenterMesh = true, normalizeScale = true, flipUVs = false;
    bool minimalFlags = false;
  };
}

// don't think we'll ever need this
#ifdef OLD_STUFF
struct MeshImportFlags
{
  MeshImportFlags() : boneWeights{ false }, animations{ false },
    lights{ false }, cameras{ false }, materials{ false } {}

  inline bool IsDefault() const noexcept { return !(boneWeights || animations || lights || cameras || materials); }
  int GetFlags() const {
    int ret{};
    if (!animations) { ret |= aiComponent_ANIMATIONS; }
    if (!boneWeights) { ret |= aiComponent_BONEWEIGHTS; }
    if (!cameras) { ret |= aiComponent_CAMERAS; }
    if (!materials) { ret |= aiComponent_MATERIALS; }
    if (!lights) { ret |= aiComponent_LIGHTS; }

    return ret;
  }

  bool boneWeights, animations, lights, cameras, materials;
};
#endif

