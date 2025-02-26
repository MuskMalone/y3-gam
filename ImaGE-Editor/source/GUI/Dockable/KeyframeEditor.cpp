#include <pch.h>
#include "KeyframeEditor.h"
#include <ImNodes/imnodes.h>
#include <GUI/Helpers/ImGuiHelpers.h>
#include <GUI/Helpers/AssetHelpers.h>
#include <Core/Systems/TransformSystem/TransformHelpers.h>
#include <ImGui/misc/cpp/imgui_stdlib.h>
#include <chrono>

#include <Core/Entity.h>
#include <Core/Components/Transform.h>
#include <Core/Components/Animation.h>
#include <GUI/GUIVault.h>
#include <Asset/IGEAssets.h>
#include <EditorEvents.h>

#include <Serialization/Serializer.h>
#include <Serialization/Deserializer.h>
#include <Events/EventManager.h>

namespace {
  struct CumulativeValues {
    CumulativeValues() : pos{}, rot{}, scale{ 1.f, 1.f, 1.f }, outdated{ true } {}

    inline void InvalidateData() noexcept { outdated = true; }

    Anim::Keyframe::ValueType prevVal;
    glm::vec3 pos, rot, scale;
    GUI::KeyframeEditor::IDType nodeId;  // the associated node
    bool outdated;
  };
}

namespace {
  static constexpr const char* sKeyframeTypeStr[]{
    "Empty", "Translation", "Scale", "Rotation"
  };
  static constexpr float sLeftColWidth = 70.f, sRightColWidth = 170.f;
  static CumulativeValues sCachedCumulativeValues;
  static GUI::KeyframeEditor::IDType sRightClickedNode;
  static std::unordered_set<GUI::KeyframeEditor::IDType> sTraversedNodes;
  static ECS::Entity sPreviewingEntity{};

  bool NodeVec3Input(const char* fieldLabel, glm::vec3& val, float min = -FLT_MAX);
  void NextRowTable(const char* labelName, float elemWidth);
}

namespace GUI {
  std::string const KeyframeEditor::sEditorFilePath = std::string(gEditorAssetsDirectory) + ".Animation\\";

  KeyframeEditor::KeyframeNode::KeyframeNode(IDType nodeId, bool hasInputPin, bool hasOutputPin) :
    data{}, nodeName{ sKeyframeTypeStr[static_cast<int>(Anim::KeyframeType::NONE)] }, nextNodes{},
    previous{}, id{ nodeId }
  {
    inputPin = hasInputPin ? ++sLastPinId : INVALID_ID;
    outputPin = hasOutputPin ? ++sLastPinId : INVALID_ID;
  }
  KeyframeEditor::KeyframeNode::KeyframeNode(IDType nodeId, Anim::Keyframe const& keyframeData, bool hasInputPin, bool hasOutputPin) :
    data{ keyframeData }, nodeName{ sKeyframeTypeStr[static_cast<int>(keyframeData.type)] }, nextNodes{},
    previous{}, id{ nodeId }
  {
    inputPin = hasInputPin ? ++sLastPinId : INVALID_ID;
    outputPin = hasOutputPin ? ++sLastPinId : INVALID_ID;
  }

  void KeyframeEditor::KeyframeNode::RemoveLinkedNode(IDType nodeId) {
    auto result{
      std::find_if(nextNodes.begin(), nextNodes.end(), [nodeId](KeyframeNode::NodePtr const& node) { return nodeId == node->id; })
    };
    if (result == nextNodes.end()) { return; }

    //std::cout << "Removed " << nodeId << " from " << id << "\n";
    (*result)->previous = nullptr;
    nextNodes.erase(result);
  }

  KeyframeEditor::KeyframeEditor(const char* windowName) : GUIWindow(windowName),
    mPinIdToNode{}, mNodes{}, mLinks{}, mRoot{}, mSelectedAnim{} {
    // the context shall live and die with this window
    ImNodes::CreateContext();
    ImNodesIO& io{ ImNodes::GetIO() };
    io.LinkDetachWithModifierClick.Modifier = &ImGui::GetIO().KeyCtrl;
    //ImNodes::PushAttributeFlag(ImNodesAttributeFlags_EnableLinkDetachWithDragClick);

    SUBSCRIBE_CLASS_FUNC(Events::EditAnimation, &KeyframeEditor::OnAnimationEdit, this);

    if (!std::filesystem::exists(sEditorFilePath) || !std::filesystem::is_directory(sEditorFilePath)) {
      std::filesystem::create_directory(sEditorFilePath);
      IGE_DBGLOGGER.LogInfo("Created directory " + sEditorFilePath);
    }
  }

  void KeyframeEditor::Run() {
    ImGui::Begin(mWindowName.c_str());

    static bool modifying{ false };
    static std::chrono::steady_clock::time_point lastModified{};
    bool modified{ false }, dataModified{ false };

    if (!mSelectedAnim) {
      ImVec2 centerPos{ ImGui::GetWindowSize() * 0.5f };
      ImGui::SetCursorPos(centerPos - ImGui::CalcTextSize("No animati"));
      ImGui::Text("No animation selected.");

      ImGui::SetCursorPosX(centerPos.x - ImGui::CalcTextSize("Edit an existing one or C").x);
      ImGui::AlignTextToFramePadding();
      ImGui::Text("Edit an existing one or");
      ImGui::SameLine();
      if (ImGui::Button("Create")) {
        NewAnimation();
        Init();
      }
      ImGui::SameLine(); ImGui::Text("one to get started.");

      ImGui::End();
      return;
    }

    if (NodesToolbar()) {

    }

    ImNodes::BeginNodeEditor();

    bool const editorHovered{ ImNodes::IsEditorHovered() };

    if (DisplayRootNode()) {
      modified = dataModified = true;
    }

    for (auto&[id, node] : mNodes) {
      bool highlight{ false };
      if (sPreviewingEntity) {
        float const timeElapsed{ sPreviewingEntity.GetComponent<Component::Animation>().timeElapsed };
        if (timeElapsed >= node->data.startTime && timeElapsed <= node->data.GetEndTime()) {
          highlight = true;
        }
      }

      if (highlight) {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.f, 0.f, 0.f, 1.f));
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(1.f, 1.f, 1.f, 1.f));
        ImNodes::PushColorStyle(ImNodesCol_TitleBar, IM_COL32(212, 175, 55, 255));
        ImNodes::PushColorStyle(ImNodesCol_TitleBarSelected, IM_COL32(212, 175, 55, 255));
        ImNodes::PushColorStyle(ImNodesCol_NodeBackground, IM_COL32(187, 161, 39, 255));
        ImNodes::PushColorStyle(ImNodesCol_NodeBackgroundSelected, IM_COL32(187, 161, 39, 255));
      }

      ImNodes::BeginNode(id);

      {
        std::ostringstream oss{};
        oss << node->nodeName << " Keyframe (t = " << std::fixed << std::setprecision(2) << node->data.startTime << "s)";

        ImNodes::BeginNodeTitleBar();
        ImGui::TextUnformatted(oss.str().c_str());
        ImNodes::EndNodeTitleBar();
      }
        
      if (KeyframeNodeBody(node)) {
        modified = dataModified = true;
      }

      if (node->inputPin != INVALID_ID) {
        ImNodes::BeginInputAttribute(node->inputPin);
        ImNodes::EndInputAttribute();
      }

      if (node->outputPin != INVALID_ID) {
        if (node->inputPin != INVALID_ID) {
          ImGui::SameLine();
        }

        ImNodes::BeginOutputAttribute(node->outputPin);
        ImNodes::EndOutputAttribute();
      }

      ImNodes::EndNode();

      if (highlight) {
        ImNodes::PopColorStyle(); ImNodes::PopColorStyle();
        ImNodes::PopColorStyle(); ImNodes::PopColorStyle();
        ImGui::PopStyleColor(2);
      }

    } // end mNodes for loop
    
    // establish the links
    for (KeyframeLink const& link : mLinks) {
      ImNodes::Link(link.linkId, link.outputPin, link.inputPin);
    }

    ImNodes::MiniMap(0.2f, ImNodesMiniMapLocation_BottomRight);
    ImNodes::EndNodeEditor();
    

    // check for link interaction
    IDType inPin, outPin, linkId;
    if (ImNodes::IsLinkDestroyed(&linkId)) {
      //IGE_DBGLOGGER.LogInfo("Destroyed");
      auto result = std::find_if(mLinks.begin(), mLinks.end(), [linkId](KeyframeLink const& link) { return link.linkId == linkId; });
      if (result != mLinks.end()) {
        inPin = result->inputPin;

        // remove the next ptr and set its prev to null
        mPinIdToNode[result->outputPin - 1]->RemoveLinkedNode(mPinIdToNode[inPin]->id);
        mLinks.erase(result);
      }
      modified = dataModified = true;
    }

    if (ImNodes::IsLinkCreated(&outPin, &inPin)) {
      KeyframeNode::NodePtr& destNode{ mPinIdToNode[inPin] };

      // only create link if current node has no previous
      if (!destNode->previous) {
        KeyframeNode::NodePtr& srcNode{ mPinIdToNode[outPin - 1] };
        //IGE_DBGLOGGER.LogInfo("Created from " + std::to_string(srcNode->id));
        mLinks.emplace_back(++sLastLinkId, inPin, outPin);
        srcNode->nextNodes.emplace_back(destNode);
        destNode->previous = srcNode;
        destNode->data.startTime = srcNode->data.GetEndTime();  // update start time

        modified = dataModified = true;
      }
    }

    // check for tooltip trigger
    IDType hoveredNodeId{};
    bool const shiftHeld{ ImGui::IsKeyDown(ImGuiKey_LeftShift) || ImGui::IsKeyDown(ImGuiKey_RightShift) };
    bool const hovered{ ImNodes::IsNodeHovered(&hoveredNodeId) };
    if (shiftHeld && hovered) {
      // ignore tooltip for root
      if (hoveredNodeId != sRootId) {
        NodePreview(mNodes[hoveredNodeId]);
      }
    }
    // if node right-clicked
    else if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && hovered) {
      sRightClickedNode = hoveredNodeId;
      ImGui::OpenPopup(sNodeOptionsPopupLabel);
    }

    // makeshift solution to consider dragging nodes around a modification
    if (hovered && ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
      modified = true;
    }

    if (!hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Right) && editorHovered) {
      ImGui::OpenPopup(sOptionsPopupLabel);
    }

    if (OptionsMenu()) {
      modified = true;
    }
    if (NodeOptionsMenu()) {
      modified = dataModified = true;
    }

    ImGui::End();

    if (sPreviewingEntity) {
      if (sPreviewingEntity.HasComponent<Component::Animation>() && sPreviewingEntity.GetComponent<Component::Animation>().timeElapsed == 0.f) {
        sPreviewingEntity = {};
      }
    }

    if (dataModified) {
      sCachedCumulativeValues.InvalidateData();
    }

    // only trigger a save after no modifications for <sTimeUntilSave> ms
    if (modified) {
      modifying = true;
      lastModified = std::chrono::steady_clock::now();
    }

    if (modifying && !modified) {
      auto const timeDiff{ std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - lastModified).count() };
      if (timeDiff >= sTimeUntilSave) {
        try {
          SaveKeyframes(IGE_ASSETMGR.GUIDToPath(mSelectedAnim));
          modifying = false;
        }
        catch (Debug::ExceptionBase&) {
          IGE_DBGLOGGER.LogError("Unable to get path of GUID: " + std::to_string(static_cast<uint64_t>(mSelectedAnim)));
          Reset();
        }
      }
    }
  }

  bool KeyframeEditor::DisplayRootNode() {
    if (!mRoot) { return false; }

    ImNodes::BeginNode(mRoot->id);
    ImNodes::BeginNodeTitleBar();
    ImGui::TextUnformatted("Root Keyframe");
    ImNodes::EndNodeTitleBar();

    ImGui::Text("Start Values");
    bool modified{ false };
    if (ImGui::BeginTable("KeyframeNodePreviewTable", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingFixedFit)) {
      ImGui::TableSetupColumn("##Col1", ImGuiTableColumnFlags_WidthFixed, sLeftColWidth);
      ImGui::TableSetupColumn("##Col2", ImGuiTableColumnFlags_WidthFixed, sRightColWidth);

      NextRowTable("Position", sRightColWidth);
      if (NodeVec3Input("##Position", mRoot->startPos)) {
        modified = true;
      }

      NextRowTable("Rotation", sRightColWidth);
      if (NodeVec3Input("##Rotation", mRoot->startRot)) {
        modified = true;
      }

      NextRowTable("Scale", sRightColWidth);
      if (NodeVec3Input("##Scale", mRoot->startScale, 0.001f)) {
        modified = true;
      }

      ImGui::EndTable();
    }

    ImNodes::BeginOutputAttribute(mRoot->outputPin);
    ImNodes::EndOutputAttribute();
    ImNodes::EndNode();

    return modified;
  }

  bool KeyframeEditor::KeyframeNodeBody(KeyframeNode::NodePtr& node) {
    bool modified{ false };

    if (ImGui::BeginTable("KeyframeNodeTable", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingFixedFit)) {
      ImGui::TableSetupColumn("##Col1", ImGuiTableColumnFlags_WidthFixed, sLeftColWidth);
      ImGui::TableSetupColumn("##Col2", ImGuiTableColumnFlags_WidthFixed, sRightColWidth);

      NextRowTable("Type", sRightColWidth);
      if (ImGui::BeginCombo("##Type", node->nodeName.c_str())) {
        for (int i{}; i < static_cast<int>(Anim::KeyframeType::NUM_TYPES); ++i) {
          if (ImGui::Selectable(sKeyframeTypeStr[i])) {
            Anim::KeyframeType const selected{ static_cast<Anim::KeyframeType>(i) };

            if (selected != node->data.type) {
              node->data.type = selected;
              node->nodeName = sKeyframeTypeStr[i];
              modified = true;

              // default value to same as root
              switch (selected) {
              case Anim::KeyframeType::TRANSLATION:
                node->data.endValue = mRoot->startPos;

                break;
              case Anim::KeyframeType::ROTATION:
                node->data.endValue = mRoot->startRot;

                break;
              case Anim::KeyframeType::SCALE:
                node->data.endValue = glm::max(mRoot->startScale, glm::vec3(0.001f));

                break;
              default:
                break;
              }
            } // end if
            break;
          } // end selectable if
        } // end for loop

        ImGui::EndCombo();
      }

      if (node->data.type != Anim::KeyframeType::NONE) {
        NextRowTable("Target", sRightColWidth);
        glm::vec3& rawVal{ std::get<glm::vec3>(node->data.endValue) };
        if (NodeVec3Input("##Target Value", rawVal, node->data.type == Anim::KeyframeType::SCALE ? 0.001f : -FLT_MAX)) {
          node->data.endValue = rawVal;
          modified = true;
        }
      }

      NextRowTable("Duration", sRightColWidth);
      if (ImGui::DragFloat("##Duration", &node->data.duration, 0.1f, 0.f, FLT_MAX, "%.2f", ImGuiSliderFlags_AlwaysClamp)) {
        UpdateStartTimesInChain(node);  // propagate changes down the chain
        modified = true;
      }
      

      ImGui::EndTable();
    }

    return modified;
  }

  void KeyframeEditor::NodePreview(KeyframeNode::NodePtr const& node) {
    GetCumulativeValues(node);

    ImGui::BeginTooltip();

    if (node->previous) {
      // starting values
      ImGui::TextUnformatted("Frame starts with:");
      if (ImGui::BeginTable("NodeTooltipTable", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingFixedFit)) {
        ImGui::TableSetupColumn("##Col1", ImGuiTableColumnFlags_WidthFixed, sLeftColWidth);
        ImGui::TableSetupColumn("##Col2", ImGuiTableColumnFlags_WidthFixed, sRightColWidth);

        NextRowTable("Position", sRightColWidth);
        NodeVec3Input("##TooltipPosition", sCachedCumulativeValues.pos);

        NextRowTable("Rotation", sRightColWidth);
        NodeVec3Input("##TooltipRotation", sCachedCumulativeValues.rot);

        NextRowTable("Scale", sRightColWidth);
        NodeVec3Input("##TooltipScale", sCachedCumulativeValues.scale);

        ImGui::EndTable();
      }

      // current offset from starting value
      if (node->data.type != Anim::KeyframeType::NONE) {
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.f);
        ImGui::TextUnformatted((node->nodeName + " offset of:").c_str());

        ImGui::SetNextItemWidth(sRightColWidth);
        // @TODO: need to change if more types are added
        switch (node->data.type) {
        case Anim::KeyframeType::TRANSLATION:
        {
          glm::vec3 offset{ std::get<glm::vec3>(node->data.endValue) - std::get<glm::vec3>(sCachedCumulativeValues.prevVal) };
          NodeVec3Input("##TooltipOffset", offset);

          break;
        }
        case Anim::KeyframeType::ROTATION:
        {
          glm::vec3 offset{ std::get<glm::vec3>(node->data.endValue) - std::get<glm::vec3>(sCachedCumulativeValues.prevVal) };
          NodeVec3Input("##TooltipOffset", offset);

          break;
        }
        case Anim::KeyframeType::SCALE:
        {
          glm::vec3 offset{ std::get<glm::vec3>(node->data.endValue) - std::get<glm::vec3>(sCachedCumulativeValues.prevVal) };
          NodeVec3Input("##TooltipOffset", offset);

          break;
        }
        default:
          break;
        }
      }
    }

    // start to end time
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.f);
    ImGui::TextUnformatted("Spans from:");
    std::ostringstream oss{};
    oss << std::fixed << std::setprecision(2) << node->data.startTime << "s  -  " << node->data.GetEndTime() << "s";
    ImGui::TextUnformatted(oss.str().c_str());

    ImGui::EndTooltip();
  }

  bool KeyframeEditor::NodesToolbar() {
    bool modified{ false };

    ImGui::Button("Help");
    if (ImGui::IsItemHovered()) {
      ImGui::BeginTooltip();

      ImGui::Text("Right-click a node to bring out context menu");
      ImGui::Text("   - The root node has different options from normal ones");
      ImGui::Text("Middle-click to pan around");
      ImGui::Text("   - You can also use the minimap to navigate around");
      ImGui::Text("Hold SHIFT while hovering over a node to see additional values");
      ImGui::Text("CTRL + Left-click a link to remap/remove it");
      ImGui::Text("The file is auto-saved after changes are made");
      ImGui::Text("Preview requires an entity with an Animation component to be selected first");

      ImGui::EndTooltip();
    }

    ImGui::SameLine();
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10.f);

    ECS::Entity selectedEntity{ GUIVault::GetSelectedEntity() };
    bool const enabled{ selectedEntity && selectedEntity.HasComponent<Component::Animation>() };
    ImGui::BeginDisabled(!enabled || sPreviewingEntity);
    if (ImGui::Button("Preview Animation")) {
      if (enabled) {
        Component::Animation& anim{ selectedEntity.GetComponent<Component::Animation>() };
        anim.timeElapsed = 0.001f;  // temp
        SaveKeyframes(IGE_ASSETMGR.GUIDToPath(mSelectedAnim));  // force a save before previewing
        QUEUE_EVENT(Events::PreviewAnimation, selectedEntity, mSelectedAnim);
        sPreviewingEntity = selectedEntity;
      }
    }
    ImGui::EndDisabled();

    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
      if (!enabled) {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.f, 0.f, 1.f));
        ImGui::SetTooltip("Select an Entity with an Animation component!");
        ImGui::PopStyleColor();
      }
      else if (sPreviewingEntity) {
        ImGui::SetTooltip("Animation in Progress...");
      }
    }

    ImGui::SameLine();
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10.f);
    if (ImGui::Button("End Session")) {
      Reset();
    }

    return modified;
  }

  bool KeyframeEditor::NodeOptionsMenu() {
    if (!ImGui::BeginPopup(sNodeOptionsPopupLabel)) { return false; }

    bool modified{ false };
    bool const isRoot{ sRightClickedNode == sRootId };

    if (isRoot) {
      if (ImGui::Selectable("Preview Keyframe")) {
        ECS::Entity currEntity{ GUIVault::GetSelectedEntity() };

        if (currEntity) {
          Component::Transform& trans{ currEntity.GetComponent<Component::Transform>() };
          trans.position = mRoot->startPos;
          trans.SetLocalRotWithEuler(mRoot->startRot);
          trans.scale = mRoot->startScale;
          trans.modified = true;

          modified = true;
        }
      }

      if (ImGui::Selectable("Use current Entity's transform")) {
        ECS::Entity const selectedEntity{ GUIVault::GetSelectedEntity() };
        if (selectedEntity) {
          Component::Transform const& trans{ selectedEntity.GetComponent<Component::Transform>() };

          mRoot->startPos = trans.position;
          mRoot->startRot = trans.eulerAngles;
          mRoot->startScale = trans.scale;

          modified = true;
        }
      }
    } // end isRoot
    else {
      if (ImGui::Selectable("Preview Keyframe")) {
        ECS::Entity currEntity{ GUIVault::GetSelectedEntity() };

        if (currEntity) {
          PreviewKeyframe(mNodes[sRightClickedNode], currEntity.GetComponent<Component::Transform>());

          modified = true;
        }
      }

      if (ImGui::Selectable("Use current Entity's transform")) {
        ECS::Entity const selectedEntity{ GUIVault::GetSelectedEntity() };
        if (selectedEntity) {
          KeyframeNode::NodePtr& rightClicked{ mNodes[sRightClickedNode] };
          switch (rightClicked->data.type) {
          case Anim::KeyframeType::TRANSLATION:
            rightClicked->data.endValue = selectedEntity.GetComponent<Component::Transform>().position;
            break;
          case Anim::KeyframeType::ROTATION:
            rightClicked->data.endValue = selectedEntity.GetComponent<Component::Transform>().eulerAngles;
            break;
          case Anim::KeyframeType::SCALE:
            rightClicked->data.endValue = selectedEntity.GetComponent<Component::Transform>().scale;
            break;
          default:
            break;
          }

          modified = true;
        }
      }

      if (ImGui::Selectable("Duplicate")) {
        KeyframeNode::NodePtr newNode{ NewNode() };
        KeyframeNode::NodePtr& rightClicked{ mNodes[sRightClickedNode] };
        newNode->data = rightClicked->data;
        newNode->data.startTime = 0.f;
        newNode->nodeName = rightClicked->nodeName;

        ImNodes::SetNodeScreenSpacePos(newNode->id, ImGui::GetMousePos());
        modified = true;
      }

      if (ImGui::Selectable("Reset")) {
        KeyframeNode::NodePtr& node{ mNodes[sRightClickedNode] };
        node->data = {};
        node->nodeName = sKeyframeTypeStr[0];

        modified = true;
      }

      if (ImGui::Selectable("Delete Keyframe")) {
        KeyframeNode::NodePtr& node{ mNodes[sRightClickedNode] };
        // remove any next ptrs
        if (!node->nextNodes.empty()) {
          for (KeyframeNode::NodePtr& next : node->nextNodes) {
            // remove the ptr reference
            next->previous = nullptr;

            // remove the link
            IDType const inPin{ next->inputPin };
            //std::cout << "Removed link with input pin " << inPin << "\n";
            std::erase_if(mLinks, [inPin](KeyframeLink const& link) { return inPin == link.inputPin; });
          }
        }

        KeyframeNode::NodePtr& prev{ node->previous };
        if (prev) {
          // remove the previous node's reference
          prev->RemoveLinkedNode(sRightClickedNode);

          // remove the link
          IDType const inPin{ node->inputPin };
          //std::cout << "Removed link with input pin " << inPin << "\n";
          std::erase_if(mLinks, [inPin](KeyframeLink const& link) { return inPin == link.inputPin; });
        }

        // remove the node
        mNodes.erase(sRightClickedNode);
        modified = true;
      }
    } // isRoot else block

    ImGui::EndPopup();
    return modified;
  }

  bool KeyframeEditor::OptionsMenu() {
    if (!ImGui::BeginPopup(sOptionsPopupLabel)) { return false; }

    bool modified{ false };
    if (ImGui::Selectable("New Keyframe")) {
      KeyframeNode::NodePtr newNode{ NewNode() };

      // create the new node at the cursor pos
      ImNodes::SetNodeScreenSpacePos(newNode->id, ImGui::GetMousePos());
      modified = true;
    }

    ImGui::EndPopup();
    return modified;
  }

  void KeyframeEditor::UpdateStartTimesInChain(KeyframeNode::NodePtr& updatedNode) {
    if (!updatedNode) { return; }

    for (KeyframeNode::NodePtr& next : updatedNode->nextNodes) {
      next->data.startTime = updatedNode->data.GetEndTime();
      UpdateStartTimesInChain(next);
    }
  }

  void KeyframeEditor::GetCumulativeValue(KeyframeNode::NodePtr const& node, Anim::KeyframeType type) const {
    if (!node || type == Anim::KeyframeType::NONE) {
      return;
    }

    // if previous node is of the same type, just set that as the prevVal
    if (node->data.type == type) {
      sCachedCumulativeValues.prevVal = node->data.endValue;
      return;
    }

    // init to root value
    switch (type) {
    case Anim::KeyframeType::TRANSLATION:
      sCachedCumulativeValues.prevVal = mRoot->startPos;

      break;
    case Anim::KeyframeType::ROTATION:
      sCachedCumulativeValues.prevVal = mRoot->startRot;

      break;
    case Anim::KeyframeType::SCALE:
      sCachedCumulativeValues.prevVal = mRoot->startScale;

      break;
    default:
      break;
    }

    // recursively check through all nodes that execute before this node
    for (KeyframeNode::NodePtr const& next : mRoot->nextNodes) {
      GetCumulativeValueR(next, node->data.startTime, 0.f, type);
    }
    sTraversedNodes.clear();
  }

  void KeyframeEditor::GetCumulativeValues(KeyframeNode::NodePtr const& node) const {
    // ignore if cached values are up to date
    if (node->id == sCachedCumulativeValues.nodeId && !sCachedCumulativeValues.outdated) {
      return;
    }

    // init to root values
    sCachedCumulativeValues.pos = mRoot->startPos;
    sCachedCumulativeValues.rot = mRoot->startRot;
    sCachedCumulativeValues.scale = mRoot->startScale;

    // vector will hold the latest time updated of each type (starts with all 0s)
    std::vector<float> latestUpdate = std::vector<float>(static_cast<int>(Anim::KeyframeType::NUM_TYPES));

    // recursively check through all nodes that execute before this node
    for (KeyframeNode::NodePtr const& next : mRoot->nextNodes) {
      GetCumulativeValuesR(next, node->data.startTime, latestUpdate);
    }
    sTraversedNodes.clear();

    // get the previous value as well
    GetCumulativeValue(node->previous, node->data.type);

    sCachedCumulativeValues.outdated = false;
    sCachedCumulativeValues.nodeId = node->id;
  }

  void KeyframeEditor::GetCumulativeValueR(KeyframeNode::NodePtr const& node, float startTime,
    float latestUpdate, Anim::KeyframeType type) const 
  {
    // base case: current node executes after the target time
    if (node->data.startTime > startTime) {
      return;
    }

    // handle loop
    if (sTraversedNodes.contains(node->id)) {
      IGE_DBGLOGGER.LogError("[AnimationEditor] LOOP DETECTED! PLEASE REMOVE THE LINK! ABORTING OPERATION...");
      return;
    }
    else {
      sTraversedNodes.emplace(node->id);
    }

    float const endTime{ std::min(node->data.GetEndTime(), startTime) };

    KeyframeData const& prevData{ node->previous->data };

    // for same KeyframeTypes, take the one with later end time
    if (endTime > latestUpdate && type == node->data.type) {
      switch (type) {
      case Anim::KeyframeType::TRANSLATION:
      {
        glm::vec3 const endValue{
          endTime < startTime ?
            node->data.GetInterpolatedValue<glm::vec3>(sCachedCumulativeValues.pos, endTime)
            : std::get<glm::vec3>(node->data.endValue)
        };

        sCachedCumulativeValues.prevVal = endValue;
        break;
      }
      case Anim::KeyframeType::ROTATION:
      {
        glm::vec3 const endValue{
          endTime < startTime ?
            node->data.GetInterpolatedValue<glm::vec3>(sCachedCumulativeValues.rot, endTime)
            : std::get<glm::vec3>(node->data.endValue)
        };

        sCachedCumulativeValues.prevVal = endValue;
        break;
      }
      case Anim::KeyframeType::SCALE:
      {
        glm::vec3 const endValue{
          endTime < startTime ?
            node->data.GetInterpolatedValue<glm::vec3>(sCachedCumulativeValues.scale, endTime)
            : std::get<glm::vec3>(node->data.endValue)
        };

        sCachedCumulativeValues.prevVal = endValue;
        break;
      }
      default:
        break;
      }

      latestUpdate = endTime;
    }

    for (KeyframeNode::NodePtr const& next : node->nextNodes) {
      GetCumulativeValueR(next, startTime, latestUpdate, type);
    }
  }

  void KeyframeEditor::GetCumulativeValuesR(KeyframeNode::NodePtr const& node,
    float startTime, std::vector<float>& latestUpdate) const 
  {
    // base case: current node executes after the target time
    if (node->data.startTime > startTime) {
      return;
    }
    
    // handle loop
    if (sTraversedNodes.contains(node->id)) {
      IGE_DBGLOGGER.LogError("[AnimationEditor] LOOP DETECTED! PLEASE REMOVE THE LINK! ABORTING OPERATION...");
      return;
    }
    else {
      sTraversedNodes.emplace(node->id);
    }

    float const endTime{ std::min(node->data.GetEndTime(), startTime) };

    // for same KeyframeTypes, take the one with later end time
    if (endTime > latestUpdate[static_cast<int>(node->data.type)]) {
      switch (node->data.type) {
      case Anim::KeyframeType::TRANSLATION:
      {
        glm::vec3 const endValue{
          endTime < startTime ?
            node->data.GetInterpolatedValue<glm::vec3>(sCachedCumulativeValues.pos, endTime)
            : std::get<glm::vec3>(node->data.endValue)
        };

        sCachedCumulativeValues.pos = endValue;
        break;
      }
      case Anim::KeyframeType::ROTATION:
      {
        glm::vec3 const endValue{
          endTime < startTime ?
            node->data.GetInterpolatedValue<glm::vec3>(sCachedCumulativeValues.rot, endTime)
            : std::get<glm::vec3>(node->data.endValue)
        };

        sCachedCumulativeValues.rot = endValue;
        break;
      }
      case Anim::KeyframeType::SCALE:
      {
        glm::vec3 const endValue{
          endTime < startTime ?
            node->data.GetInterpolatedValue<glm::vec3>(sCachedCumulativeValues.scale, endTime)
            : std::get<glm::vec3>(node->data.endValue)
        };

        sCachedCumulativeValues.scale = endValue;
        break;
      }
      default:
        break;
      }

      latestUpdate[static_cast<int>(node->data.type)] = endTime;
    }

    for (KeyframeNode::NodePtr const& next : node->nextNodes) {
      GetCumulativeValuesR(next, startTime, latestUpdate);
    }
  }
  
  void KeyframeEditor::PreviewKeyframe(KeyframeNode::NodePtr const& node, Component::Transform& transform) const {
    GetCumulativeValues(node);

    switch (node->data.type) {
    case Anim::KeyframeType::TRANSLATION:
      sCachedCumulativeValues.pos = std::get<glm::vec3>(node->data.endValue);

      break;
    case Anim::KeyframeType::ROTATION:
      sCachedCumulativeValues.rot = std::get<glm::vec3>(node->data.endValue);

      break;
    case Anim::KeyframeType::SCALE:
      sCachedCumulativeValues.scale = std::get<glm::vec3>(node->data.endValue);

      break;
    default:
      break;
    }

    transform.position = sCachedCumulativeValues.pos;
    transform.SetLocalRotWithEuler(sCachedCumulativeValues.rot);
    transform.scale = sCachedCumulativeValues.scale;
    transform.modified = true;
  }

  void KeyframeEditor::Init() {
    mRoot = std::make_shared<RootKeyframeNode>();

    mPinIdToNode.emplace(mRoot->outputPin - 1, mRoot);
  }

  void KeyframeEditor::InitRoot(Anim::RootKeyframe const& keyframe) {
    mRoot->startPos = keyframe.startPos;
    mRoot->startRot = keyframe.startRot;
    mRoot->startScale = keyframe.startScale;
  }

  EVENT_CALLBACK_DEF(KeyframeEditor, OnAnimationEdit) {
    Reset();
    Init();
    LoadKeyframes(CAST_TO_EVENT(Events::EditAnimation)->mGUID);
  }

  bool KeyframeEditor::LoadKeyframes(IGE::Assets::GUID guid) {
    if (!guid) { return false; }

    mSelectedAnim = guid;
    IGE::Assets::AssetManager& am{ IGE_ASSETMGR };
    try {
      am.LoadRef<IGE::Assets::AnimationAsset>(guid);
    }
    catch (Debug::ExceptionBase&) {
      IGE_DBGLOGGER.LogError("[AnimationSystem] Unable to get animation " +
        std::to_string(static_cast<uint64_t>(guid)));
      return false;
    }

    Anim::AnimationData const& animData{ am.GetAsset<IGE::Assets::AnimationAsset>(guid)->mAnimData };
    InitRoot(animData.rootKeyframe);

    // dummy root node to pass into the recursive function
    Anim::Node dummy{ std::make_shared<Anim::Keyframe>() };
    dummy->nextNodes = animData.rootKeyframe.nextNodes;

    std::unordered_map<IDType, IDType> idMap;
    {
      KeyframeNode::NodePtr rootPtr{ mRoot };
      CloneKeyframeTree(rootPtr, dummy, idMap);
    }

    // deserialize node positions
    auto& metadata{ am.GetMetadata<IGE::Assets::AnimationAsset>(guid).metadata };
    NodePositions nodePositions{};
    if (metadata.contains(sMetadataKey)) {
      Serialization::Deserializer::DeserializeAny(nodePositions, metadata[sMetadataKey]);
    }
    else {
      std::string const& path{ am.GUIDToPath(mSelectedAnim) };
      IGE_DBGLOGGER.LogError("[AnimationEditor] Unable to find editor file for animation " + path + ". New file created.");

      CreateEditorFile(path);
    }

    // set root node pos
    if (nodePositions.posMap.contains(sRootId)) {
      ImNodes::SetNodeGridSpacePos(sRootId, nodePositions.posMap.at(sRootId));
    }

    // restore the positions based on their old IDs
    for (auto const& [id, pos] : nodePositions.posMap) {
      ImNodes::SetNodeGridSpacePos(idMap[id], nodePositions.posMap.at(id));
    }

    return true;
  }

  void KeyframeEditor::SaveKeyframes(std::string const& filePath) const {
    IGE::Assets::AssetManager& am{ IGE_ASSETMGR };
    NodePositions nodePositions;
    nodePositions.posMap.reserve(mNodes.size());

    // first, create the new keyframe tree
    Anim::AnimationData& animData{ am.GetAsset<IGE::Assets::AnimationAsset>(mSelectedAnim)->mAnimData };

    // create the root first
    animData.rootKeyframe = mRoot->ToRootKeyframe();
    nodePositions.posMap.emplace(sRootId, ImNodes::GetNodeGridSpacePos(sRootId));

    // construct the rest of the tree
    for (KeyframeNode::NodePtr const& next : mRoot->nextNodes) {
      KeyframeData const& data{ next->data };
      Anim::Node newKeyframe{ std::make_shared<Anim::Keyframe>(data.endValue, data.type, data.startTime, data.duration) };
      newKeyframe->id = next->id;
      animData.rootKeyframe.nextNodes.emplace_back(newKeyframe);
      nodePositions.posMap.emplace(next->id, ImNodes::GetNodeGridSpacePos(next->id));

      CreateOutputTree(newKeyframe, next, nodePositions.posMap);
    }

    // finally, save to file
    Serialization::Serializer::SerializeAnimationData(animData, filePath);

    // save the positions to the path saved in metadata
    auto& metadata{ am.GetMetadata<IGE::Assets::AnimationAsset>(mSelectedAnim).metadata };
    if (!metadata.contains(sMetadataKey)) {
      std::string const& path{ am.GUIDToPath(mSelectedAnim) };
      IGE_DBGLOGGER.LogError("[AnimationEditor] Unable to find editor file for animation " + path + ". New file created.");

      CreateEditorFile(path);
    }
    Serialization::Serializer::SerializeAny(nodePositions, metadata[sMetadataKey]);
  }

  KeyframeEditor::KeyframeNode::NodePtr KeyframeEditor::NewNode() {
    KeyframeNode::NodePtr newNode{
        std::make_shared<KeyframeNode>(KeyframeNode::NextID(), true, true)
    };

    mPinIdToNode.emplace(newNode->inputPin, newNode);
    mNodes.emplace(newNode->id, newNode);

    return newNode;
  }

  void KeyframeEditor::NewAnimation() {
    std::string extensions{ std::string("animation (*") + gAnimationFileExt + ")" };
    { // workaround to set a null-terminating char in the middle of str
      std::string::size_type toReplace{ extensions.size() };
      extensions += std::string("a*") + gAnimationFileExt;
      extensions[toReplace] = '\0';
      extensions.append(2, '\0');
    }

    std::string const newFile{ AssetHelpers::SaveFileToExplorer(gAnimationFileExt, "New Animation.anim",
      extensions.c_str(), 2, gAnimationsDirectory)};
    if (newFile.empty()) { return; }

    try {
      IGE::Assets::AssetManager& am{ IGE_ASSETMGR };
      mSelectedAnim = am.ImportAsset<IGE::Assets::AnimationAsset>(newFile);
      am.LoadRef<IGE::Assets::AnimationAsset>(mSelectedAnim);

      CreateEditorFile(newFile);
    }
    catch (Debug::ExceptionBase&) {
      IGE_DBGLOGGER.LogError("Error creating new animation: " + newFile);
    }
  }
  
  void KeyframeEditor::CreateEditorFile(std::string const& animFile) const {
    // find a free filename
    std::string editorFilePath{ sEditorFilePath + std::filesystem::path(animFile).stem().string() };
    if (std::filesystem::exists(editorFilePath)) {
      std::string temp{ editorFilePath };
      for (int i{ 2 }; std::filesystem::exists(temp); ++i) {
        temp = editorFilePath + std::to_string(i);
      }

      editorFilePath = std::move(temp);
    }
    // insert into metadata
    auto& metadata{ IGE_ASSETMGR.GetMetadata<IGE::Assets::AnimationAsset>(mSelectedAnim) };
    metadata.metadata[sMetadataKey] = editorFilePath;
    metadata.modified = true;
  }

  void KeyframeEditor::CloneKeyframeTree(KeyframeNode::NodePtr& dest, Anim::Node const& src, std::unordered_map<IDType, IDType>& posMap) {
    // recursively construct the editor tree from the source tree
    for (Anim::Node const& next : src->nextNodes) {
      KeyframeNode::NodePtr newNode{
        std::make_shared<KeyframeNode>(KeyframeNode::NextID(), *next)
      };
      newNode->previous = dest;
      dest->nextNodes.emplace_back(newNode);
      mNodes.emplace(newNode->id, newNode);

      // create link
      mLinks.emplace_back(++sLastLinkId, newNode->inputPin, dest->outputPin);
      mPinIdToNode.emplace(newNode->inputPin, newNode);

      CloneKeyframeTree(newNode, next, posMap);
    }

    // map old IDs to new ones
    posMap.emplace(src->id, dest->id);
  }

  void KeyframeEditor::CreateOutputTree(Anim::Node& dest, KeyframeNode::NodePtr const& src, NodePosMap& nodePosMap) const {
    // recursively construct the new tree from the editor tree
    for (KeyframeNode::NodePtr const& next : src->nextNodes) {
      KeyframeData const& data{ next->data };
      Anim::Node newNode{
        std::make_shared<Anim::Keyframe>(data.endValue, data.type, data.startTime, data.duration)
      };
      newNode->id = next->id;
      dest->nextNodes.emplace_back(newNode);
      nodePosMap.emplace(next->id, ImNodes::GetNodeGridSpacePos(next->id));

      CreateOutputTree(newNode, next, nodePosMap);
    }
  }

  Anim::RootKeyframe KeyframeEditor::RootKeyframeNode::ToRootKeyframe() const {
    Anim::RootKeyframe ret{};

    ret.startPos = startPos;
    ret.startRot = startRot;
    ret.startScale = startScale;

    return ret;
  }

  void KeyframeEditor::Reset() {
    mRoot.reset();
    mSelectedAnim = {};
    mNodes.clear();
    mLinks.clear();
    mPinIdToNode.clear();
    KeyframeNode::ResetIDs();
    sCachedCumulativeValues = {};
  }

  KeyframeEditor::~KeyframeEditor() {
    ImNodes::DestroyContext();
  }
} // namespace GUI

namespace {
  bool NodeVec3Input(const char* fieldLabel, glm::vec3& val, float min) {
    return ImGui::DragFloat3(fieldLabel, glm::value_ptr(val), 0.1f, min, FLT_MAX, "%.2f");
  }

  void NextRowTable(const char* labelName, float elemWidth) {
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::Text(labelName);
    ImGui::TableSetColumnIndex(1);
    ImGui::SetNextItemWidth(elemWidth);
  }
}
