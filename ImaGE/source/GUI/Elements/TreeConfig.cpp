#include <pch.h>
#include "TreeConfig.h"
#include <imgui/imgui.h>

#define INITIAL_SUBMISSION

namespace GUI
{

  TreeConfig::TreeConfig(std::string const& windowName, Scene& scene, BV::VolumeBounder& bvh)
    : GUIWindow(windowName, scene), m_bvh{ bvh }
  {

  }

  void TreeConfig::Run()
  {
    ImGui::Begin(GetName().c_str());

#ifdef INITIAL_SUBMISSION
    ImGui::NewLine();
    ImGui::Text("Display Tree");
    ImGui::SameLine();
    ImGui::Checkbox("##treeDisplay", &Settings::showTree);

    if (Settings::currDataStructure == Settings::DataStructure::OCTREE)
    {
      ImGui::NewLine();
      ImGui::Text("Display Tree BV");
      ImGui::SameLine();
      ImGui::Checkbox("##showTreeBV", &Settings::displayTreeBV);
    }

    static const char* dsNames[]{
      "BVH", "Octree", "BSP Tree"
    };
    static const char* splitPlaneMethodNames[]{
      "Autopartitioning", "Alternate Autopartitioning"
    };
    static Settings::BSPSplitPlaneMethod currMethod = Settings::BSPSplitPlaneMethod::AUTOPARTITIONING;
    ImGui::NewLine();
    ImGui::Text("Tree Type");
    if (ImGui::BeginCombo("##TreeTypeDropdown", dsNames[static_cast<int>(Settings::currDataStructure)]))
    {
      for (int i{ 1 }; i < static_cast<int>(Settings::DataStructure::NUM_TYPES); ++i)
      {
        if (ImGui::Selectable(dsNames[i]))
        {
          Settings::currDataStructure = static_cast<Settings::DataStructure>(i);
        }
      }

      ImGui::EndCombo();
    }

    ImGui::NewLine();
    ImGui::Text("Termination Criteria");
    if (Settings::currDataStructure == Settings::DataStructure::OCTREE)
    {
      if (ImGui::InputInt("##TerminationCriteria", &Settings::trianglesPerOctTreeCell, 100, 200)) {
        if (Settings::trianglesPerOctTreeCell < 100) { Settings::trianglesPerOctTreeCell = 100; }
        m_sceneRef.ReconstructTree();
      }
    }
    else if (Settings::currDataStructure == Settings::DataStructure::BSPTREE)
    {
      if (ImGui::InputInt("##TerminationCriteria", &Settings::trianglesPerBSPTreeCell, 100, 200)) {
        if (Settings::trianglesPerBSPTreeCell < 100) { Settings::trianglesPerBSPTreeCell = 100; }
        m_sceneRef.ReconstructTree();
      }

      ImGui::NewLine();
      ImGui::Text("Split Plane Approach");
      if (ImGui::BeginCombo("##AplitPlaneDropdown", splitPlaneMethodNames[static_cast<int>(currMethod)]))
      {
        for (int i{}; i < static_cast<int>(Settings::BSPSplitPlaneMethod::NUM_METHODS); ++i)
        {
          if (ImGui::Selectable(splitPlaneMethodNames[i]))
          {
            currMethod = static_cast<Settings::BSPSplitPlaneMethod>(i);
          }
        }

        ImGui::EndCombo();
      }
    }
#else
    }
    static bool bvhWarning{ false };
    ImGui::NewLine();
    ImGui::Text("Show BVH");
    ImGui::SameLine();
    if (ImGui::Checkbox("##bvh", &Settings::showTree)) {
      if (Settings::bvType >= BV::BVType::OBB) {
        bvhWarning = true;
        Settings::showTree = false;
      }
      else {
        bvhWarning = false;
      }
    }

    if (bvhWarning)
    {
      ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "BVH Unavailable for");
      ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "OBBs and Ellipsoids!");
    }

    if (!Settings::showTree) { ImGui::BeginDisabled(); }
    ImGui::NewLine();
    ImGui::Text("Construction Method");
    static constexpr const char* methodStr[]{ "Top-down", "Bottom-up" };
    if (ImGui::BeginCombo("##constructionMethods", methodStr[static_cast<int>(m_bvh.GetCurrentType())]))
    {
      for (int i{}; i < static_cast<int>(BV::BVHierarchy::Type::NUM_TYPES); ++i)
      {
        if (ImGui::Selectable(methodStr[i]))
        {
          m_sceneRef.ChangeConstructionMethod(static_cast<BV::BVHierarchy::Type>(i));
          break;
        }
      }

      ImGui::EndCombo();
    }

    if (m_bvh.GetCurrentType() == BV::BVHierarchy::Type::TOP_DOWN)
    {
      ImGui::NewLine();
      ImGui::Text("Split Point Method");
      static constexpr const char* splitPointMethodStr[]{ "Median of Centers", "Median of Extents", "K-even Splits" };
      if (ImGui::BeginCombo("##splitPointMethods", splitPointMethodStr[static_cast<int>(Settings::splitMethod)]))
      {
        for (int i{}; i < static_cast<int>(BV::SplitMethod::NUM_METHODS); ++i)
        {
          if (ImGui::Selectable(splitPointMethodStr[i]))
          {
            auto chosenMethod{ static_cast<BV::SplitMethod>(i) };
            if (chosenMethod != Settings::splitMethod)
            {
              Settings::splitMethod = static_cast<BV::SplitMethod>(i);
              m_sceneRef.RecomputeBVH();
            }
            break;
          }
        }

        ImGui::EndCombo();
      }
    }
    // else bottom-up
    else
    {
      bool heuristicChanged{ false };
      int const heuristicCount{
        (Settings::combinedVolume ? 1 : 0) + (Settings::nearestNeighbor ? 1
        : 0) + (Settings::relativeIncrease ? 1 : 0)
      };
      ImGui::NewLine();

      ImGui::Text("Nearest Neighbor            ");
      ImGui::SameLine();
      if (ImGui::Checkbox("##nearestNeighbor", &Settings::nearestNeighbor)) {
        if (heuristicCount == 1 && !Settings::nearestNeighbor) {
          Settings::nearestNeighbor = true;
        }
        else {
          heuristicChanged = true;
        }
      }

      ImGui::Text("Min Combined Volume         ");
      ImGui::SameLine();
      if (ImGui::Checkbox("##minCombinedVol", &Settings::combinedVolume)) {
        if (heuristicCount == 1 && !Settings::combinedVolume) {
          Settings::combinedVolume = true;
        }
        else {
          heuristicChanged = true;
        }
      }
    
      ImGui::Text("Min Relative Increase in Vol");
      ImGui::SameLine();
      if (ImGui::Checkbox("##minRelIncrVol", &Settings::relativeIncrease)) {
        if (heuristicCount == 1 && !Settings::relativeIncrease) {
          Settings::relativeIncrease = true;
        }
        else {
          heuristicChanged = true;
        }
      }

      if (heuristicChanged) { m_sceneRef.RecomputeBVH(); }
    }

    ImGui::NewLine();
    ImGui::Text("Show BVH Levels");
    ImGui::Separator();
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 1.f);
    ImGui::Text("Levels 7 & Up (Teal) ");
    ImGui::SameLine();
    ImGui::Checkbox("##bvhOtherLevels", &Settings::bvhOtherLevelsVisible);

    static const char* bvhClrs[]{
      " (Red)        ", " (Orange)     ", " (Yellow)     ", " (Green)      ", " (Blue)       ", " (Purple)     ", " (Pink)       "
    };
    for (int i{ 6 }; i >= 0; --i)
    {
      std::string const num{ std::to_string(i) };
      ImGui::Text(("Level " + num + bvhClrs[i]).c_str());
      ImGui::SameLine();
      ImGui::Checkbox(("##bvhLevel" + num).c_str(), Settings::bvhLevelVisibility + i);
    }
    if (!Settings::showTree) { ImGui::EndDisabled(); }
#endif

    ImGui::End();
  }

} // namespace GUI
