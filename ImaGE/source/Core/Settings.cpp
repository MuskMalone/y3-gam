#include <pch.h>
#include "Settings.h"

float                     Settings::clearClr[4]               = {};
glm::vec4                 Settings::objectClr                 = { 1.f, 1.f, 1.f, 1.f };
glm::vec4                 Settings::collidedClr               = { 1.f, 0.f, 0.f, 1.f };
float                     Settings::pointSize                 = 1.f;
float                     Settings::lineWidth                 = 1.f;
int                       Settings::trianglesPerOctTreeCell   = 300;
int                       Settings::trianglesPerBSPTreeCell   = 300;
Settings::RenderMode      Settings::renderMode                = Settings::RenderMode::WIREFRAME;
Settings::DataStructure   Settings::currDataStructure         = Settings::DataStructure::OCTREE;
BV::BVType                Settings::bvType                    = BV::BVType::AABB;
BV::SplitMethod           Settings::splitMethod               = BV::SplitMethod::MEDIAN_CENTER;
bool                      Settings::enableDepthBuffer         = true;
bool                      Settings::backfaceCulling           = false;
bool                      Settings::showBoundingVol           = false;
bool                      Settings::displayTreeBV             = true;
bool                      Settings::showTree                  = false;
bool                      Settings::nearestNeighbor           = true;
bool                      Settings::combinedVolume            = true;
bool                      Settings::relativeIncrease          = true;
bool                      Settings::bvhLevelVisibility[7]     = { true, true, true, true, true, true, true };
bool                      Settings::bvhOtherLevelsVisible     = true;


void Settings::Init(glm::vec4 const& clearColor, float ptSize)
{
  SetRenderMode(renderMode);
  SetClearColor(clearColor);
  SetPointSize(pointSize);
  SetLineWidth(lineWidth);
}

void Settings::SetRenderMode(RenderMode mode)
{
  renderMode = mode;
  switch (mode)
  {
  case RenderMode::POINTS:
    glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
    break;
  case RenderMode::WIREFRAME:
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    break;
  case RenderMode::FILL:
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    break;
  default:
    throw std::runtime_error("Invalid RenderMode!");
  }
}

void Settings::SetClearColor(glm::vec4 const& clr)
{
  glClearColor(clr.r, clr.g, clr.b, clr.a);
  clearClr[0] = clr.r;
  clearClr[1] = clr.g;
  clearClr[2] = clr.b;
  clearClr[3] = clr.a;
}

void Settings::SetPointSize(float size)
{
  pointSize = size;
  glPointSize(size);
}

void Settings::SetLineWidth(float size)
{
  lineWidth = size;
  glLineWidth(size);
}
