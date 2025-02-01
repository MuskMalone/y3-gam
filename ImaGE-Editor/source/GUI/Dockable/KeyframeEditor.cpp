#include <pch.h>
#include "KeyframeEditor.h"
#include <ImNodes/imnodes.h>
#include <GUI/Helpers/ImGuiHelpers.h>
#include <GUI/Helpers/AssetHelpers.h>
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
  static constexpr const char* sKeyframeTypeStr[]{
    "Empty", "Translation", "Scale", "Rotation"
  };
  static constexpr float sLeftColWidth = 70.f, sRightColWidth = 170.f;
  static GUI::KeyframeEditor::IDType sRightClickedNode;
  static std::unordered_set<GUI::KeyframeEditor::IDType> sTraversedNodes;
  static ECS::Entity sPreviewingEntity{};

  bool NodeVec3Input(const char* fieldLabel, glm::vec3& val, float min = -FLT_MAX);
  void NextRowTable(const char* labelName, float elemWidth);
}

namespace GUI {
  std::string const KeyframeEditor::sEditorFilePath = std::string(gEditorAssetsDirectory) + ".Animation\\";

  KeyframeEditor::KeyframeNode::KeyframeNode(IDType nodeId, Anim::Keyframe const& keyframeData, bool hasInputPin, bool hasOutputPin) :
    data{ keyframeData }, cumulativeVal{}, nodeName{ sKeyframeTypeStr[static_cast<int>(keyframeData.type)] }, nextNodes{},
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
    mPinIdToNode{}, mNodes{}, mLinks{}, mSelectedAnim{} {
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
    bool modified{ false };

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

      if (id == sRootId) {
        DisplayRootNode(node);
      }
      else {
        {
          std::ostringstream oss{};
          oss << node->nodeName << " Keyframe (t = " << std::fixed << std::setprecision(2) << node->data.startTime << "s)";

          ImNodes::BeginNodeTitleBar();
          ImGui::TextUnformatted(oss.str().c_str());
          ImNodes::EndNodeTitleBar();
        }
        
        if (KeyframeNodeBody(node)) {
          UpdateChain(node);
          modified = true;
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
      } // end else

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

      OptionsMenu();
      NodeOptionsMenu();
    

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
      modified = true;
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
        UpdateChain(destNode);
        modified = true;
      }
    }

    ImGui::End();

    if (sPreviewingEntity) {
      if (sPreviewingEntity.HasComponent<Component::Animation>() && sPreviewingEntity.GetComponent<Component::Animation>().timeElapsed == 0.f) {
        sPreviewingEntity = {};
      }
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

  bool KeyframeEditor::DisplayRootNode(KeyframeNode::NodePtr const& root) {
    ImNodes::BeginNodeTitleBar();
    ImGui::TextUnformatted("Root Keyframe");
    ImNodes::EndNodeTitleBar();

    ImGui::Text("Start Values");
    bool modified{ false };
    if (ImGui::BeginTable("KeyframeNodePreviewTable", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingFixedFit)) {
      ImGui::TableSetupColumn("##Col1", ImGuiTableColumnFlags_WidthFixed, sLeftColWidth);
      ImGui::TableSetupColumn("##Col2", ImGuiTableColumnFlags_WidthFixed, sRightColWidth);
      CumulativeValues& vals{ root->cumulativeVal };

      NextRowTable("Position", sRightColWidth);
      if (NodeVec3Input("##Position", vals.pos)) {
        modified = true;
      }

      NextRowTable("Rotation", sRightColWidth);
      if (NodeVec3Input("##Rotation", vals.rot)) {
        modified = true;
      }

      NextRowTable("Scale", sRightColWidth);
      if (NodeVec3Input("##Scale", vals.scale, 0.001f)) {
        modified = true;
      }

      ImGui::EndTable();
    }

    if (root->outputPin != INVALID_ID) {
      ImNodes::BeginOutputAttribute(root->outputPin);
      ImNodes::EndOutputAttribute();
    }

    return modified;
  }

  bool KeyframeEditor::KeyframeNodeBody(KeyframeNode::NodePtr const& node) {
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

              if (selected == Anim::KeyframeType::SCALE) {
                node->data.endValue = glm::clamp(std::get<glm::vec3>(node->data.endValue), glm::vec3(0.001f), glm::vec3(FLT_MAX));
              }
            }
            break;
          }
        }

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
        modified = true;
      }
      

      ImGui::EndTable();
    }

    return modified;
  }

  void KeyframeEditor::NodePreview(KeyframeNode::NodePtr const& node) {
    CumulativeValues& values{ node->cumulativeVal };
    ImGui::BeginTooltip();

    // starting values
    ImGui::TextUnformatted("Frame starts with:");
    if (ImGui::BeginTable("NodeTooltipTable", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingFixedFit)) {
      ImGui::TableSetupColumn("##Col1", ImGuiTableColumnFlags_WidthFixed, sLeftColWidth);
      ImGui::TableSetupColumn("##Col2", ImGuiTableColumnFlags_WidthFixed, sRightColWidth);

      NextRowTable("Position", sRightColWidth);
      NodeVec3Input("##TooltipPosition", values.pos);

      NextRowTable("Rotation", sRightColWidth);
      NodeVec3Input("##TooltipRotation", values.rot);

      NextRowTable("Scale", sRightColWidth);
      NodeVec3Input("##TooltipScale", values.scale);

      ImGui::EndTable();
    }

    // current offset from starting value
    if (node->data.type != Anim::KeyframeType::NONE) {
      ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.f);
      ImGui::TextUnformatted((node->nodeName + " offset of:").c_str());

      ImGui::SetNextItemWidth(sRightColWidth);
      // @TODO: need to change if more types are added
      glm::vec3 offset{ node->data.GetNormalizedValue<glm::vec3>() };
      NodeVec3Input("##TooltipOffset", offset);
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
    if (ImGui::Button("Preview Animation")) {
      if (enabled) {
        Component::Animation& anim{ selectedEntity.GetComponent<Component::Animation>() };
        anim.timeElapsed = 0.001f;  // temp
        QUEUE_EVENT(Events::PreviewAnimation, selectedEntity, mSelectedAnim);
        sPreviewingEntity = selectedEntity;
      }
    }
    if (ImGui::IsItemHovered() && !enabled) {
      ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.f, 0.f, 1.f));
      ImGui::SetTooltip("Select an Entity with an Animation component!");
      ImGui::PopStyleColor();
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
      if (ImGui::Selectable("Use current Entity's transform")) {
        ECS::Entity const selectedEntity{ GUIVault::GetSelectedEntity() };
        if (selectedEntity) {
          CumulativeValues& rootVals{ GetRootNode()->cumulativeVal };
          Component::Transform const& trans{ selectedEntity.GetComponent<Component::Transform>() };

          rootVals.pos = trans.position;
          rootVals.rot = trans.eulerAngles;
          rootVals.scale = trans.scale;

          // propagate the changes down the root
          for (KeyframeNode::NodePtr& next : GetRootNode()->nextNodes) {
            UpdateChain(next);
          }

          modified = true;
        }
      }
    } // end isRoot
    else {
      if (ImGui::Selectable("Reset")) {
        KeyframeNode::NodePtr& node{ mNodes[sRightClickedNode] };
        node->data = {};
        node->nodeName = sKeyframeTypeStr[0];
        UpdateChain(node);

        modified = true;
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

  void KeyframeEditor::UpdateChainR(KeyframeNode::NodePtr const& node) {
    if (!node->previous) {
      node->cumulativeVal = {};
      return;
    }
    
    if (sTraversedNodes.contains(node->id)) {
      IGE_DBGLOGGER.LogError("[AnimationEditor] LOOP DETECTED! PLEASE REMOVE THE LINK! ABORTING OPERATION...");
      return;
    }
    else {
      sTraversedNodes.emplace(node->id);
    }

    KeyframeData const& prevData{ node->previous->data };
    node->cumulativeVal = node->previous->cumulativeVal;
    node->data.startTime = prevData.GetEndTime();

    switch (prevData.type) {
    case Anim::KeyframeType::TRANSLATION:
      node->cumulativeVal.pos = std::get<glm::vec3>(prevData.endValue);
      break;
    case Anim::KeyframeType::ROTATION:
      node->cumulativeVal.rot = std::get<glm::vec3>(prevData.endValue);
      break;
    case Anim::KeyframeType::SCALE:
      node->cumulativeVal.scale = std::get<glm::vec3>(prevData.endValue);
      break;
    default:
      break;
    }

    switch (node->data.type) {
    case Anim::KeyframeType::TRANSLATION:
      node->data.startValue = node->cumulativeVal.pos;
      break;
    case Anim::KeyframeType::ROTATION:
      node->data.startValue = node->cumulativeVal.rot;
      break;
    case Anim::KeyframeType::SCALE:
      node->data.startValue = node->cumulativeVal.scale;
      break;
    default:
      break;
    }

    for (KeyframeNode::NodePtr const& next : node->nextNodes) {
      UpdateChain(next);
    }
  }

  void KeyframeEditor::UpdateChain(KeyframeNode::NodePtr const& node) {
    UpdateChainR(node);
    sTraversedNodes.clear();
  }

  void KeyframeEditor::Init() {
    Reset();

    KeyframeNode::NodePtr newNode{
      std::make_shared<KeyframeNode>(sRootId, Anim::Keyframe(), false, true)
    };
    newNode->nodeName = "Root";

    mPinIdToNode.emplace(newNode->outputPin - 1, newNode);
    mNodes.emplace(sRootId, std::move(newNode));
  }

  void KeyframeEditor::InitRoot(Anim::RootKeyframe const& keyframe) {
    KeyframeNode::NodePtr const& root{ GetRootNode() };
    root->cumulativeVal.pos = keyframe.startPos;
    root->cumulativeVal.rot = keyframe.startRot;
    root->cumulativeVal.scale = keyframe.startScale;
  }

  EVENT_CALLBACK_DEF(KeyframeEditor, OnAnimationEdit) {
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
    CloneKeyframeTree(GetRootNode(), dummy, idMap);

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
    KeyframeNode::NodePtr const& root{ GetRootNode() };
    animData.rootKeyframe = CreateRootKeyframe(root);
    root->id = sRootId;
    nodePositions.posMap.emplace(sRootId, ImNodes::GetNodeGridSpacePos(sRootId));

    // construct the rest of the tree
    for (KeyframeNode::NodePtr const& next : root->nextNodes) {
      KeyframeData const& data{ next->data };
      Anim::Node newKeyframe{ std::make_shared<Anim::Keyframe>(data.startValue, data.endValue, data.type, data.startTime, data.duration) };
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
        std::make_shared<KeyframeNode>(KeyframeNode::NextID(), Anim::Keyframe(), true, true)
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
        std::make_shared<Anim::Keyframe>(data.startValue, data.endValue, data.type, data.startTime, data.duration)
      };
      newNode->id = next->id;
      dest->nextNodes.emplace_back(newNode);
      nodePosMap.emplace(next->id, ImNodes::GetNodeGridSpacePos(next->id));

      CreateOutputTree(newNode, next, nodePosMap);
    }
  }

  Anim::RootKeyframe KeyframeEditor::CreateRootKeyframe(KeyframeNode::NodePtr const& root) const {
    CumulativeValues const& vals{ root->cumulativeVal };
    Anim::RootKeyframe ret{};

    ret.startPos = vals.pos;
    ret.startRot = vals.rot;
    ret.startScale = vals.scale;

    return ret;
  }

  void KeyframeEditor::Reset() {
    mSelectedAnim = {};
    mNodes.clear();
    mLinks.clear();
    mPinIdToNode.clear();
    KeyframeNode::ResetIDs();
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
