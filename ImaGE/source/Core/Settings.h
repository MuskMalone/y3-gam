#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <BoundingVolumes/BVTypes.h>
#include <BVH/SplitMethod.h>

struct Settings
{
  enum RenderMode
  {
    POINTS = 0,
    WIREFRAME,
    FILL,
    NUM_MODES
  };

  enum DataStructure
  {
    BVH = 0,
    OCTREE,
    BSPTREE,
    NUM_TYPES
  };

  enum BSPSplitPlaneMethod
  {
    AUTOPARTITIONING,
    ALTERNATE,
    NUM_METHODS
  };

  static float clearClr[4];
  static glm::vec4 objectClr, collidedClr;
  static float pointSize, lineWidth;
  static int trianglesPerOctTreeCell;
  static int trianglesPerBSPTreeCell;

  static RenderMode renderMode;
  static DataStructure currDataStructure;
  static BV::BVType bvType;
  static BV::SplitMethod splitMethod;

  static bool enableDepthBuffer, backfaceCulling, showBoundingVol, showTree;
  // bottom-up heuristics
  static bool nearestNeighbor, combinedVolume, relativeIncrease;
  static bool bvhLevelVisibility[7];
  static bool bvhOtherLevelsVisible;
  static bool displayTreeBV;

  static constexpr const char* renderModeNames[] { "Points", "Wireframe", "Fill" };

  static void Init(glm::vec4 const& clearColor, float ptSize);
  static void SetRenderMode(RenderMode mode);
  static void SetClearColor(glm::vec4 const& clr);
  static void SetPointSize(float size);
  static void SetLineWidth(float size);
};
