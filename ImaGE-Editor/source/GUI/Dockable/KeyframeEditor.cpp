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

  bool NodeVec3Input(const char* fieldLabel, glm::vec3& val);
  void NextRowTable(const char* labelName, float elemWidth);
}

namespace GUI {
  KeyframeEditor::KeyframeNode::KeyframeNode(Anim::Keyframe const& keyframeData, bool hasInputPin, bool hasOutputPin) :
    data{ keyframeData }, cumulativeVal{}, nodeName{ sKeyframeTypeStr[static_cast<int>(keyframeData.type)] }, nextNodes{},
    previous{}
  {
    inputPin = hasInputPin ? ++sLastPinId : INVALID_ID;
    outputPin = hasOutputPin ? ++sLastPinId : INVALID_ID;
  }

  KeyframeEditor::KeyframeEditor(const char* windowName) : GUIWindow(windowName),
    mPinIdToNode{}, mNodes{}, mLinks{}, mSelectedAnim{} {
    // the context shall live and die with this window
    ImNodes::CreateContext();
    ImNodesIO& io{ ImNodes::GetIO() };
    io.LinkDetachWithModifierClick.Modifier = &ImGui::GetIO().KeyCtrl;
    //ImNodes::PushAttributeFlag(ImNodesAttributeFlags_EnableLinkDetachWithDragClick);

    SUBSCRIBE_CLASS_FUNC(Events::EditAnimation, &KeyframeEditor::OnAnimationEdit, this);
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

    ECS::Entity currEntity{ GUIVault::GetSelectedEntity() };

    if (NodesToolbar()) {
      
    }

    ImNodes::BeginNodeEditor();

    for (auto&[id, node] : mNodes) {
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
    }
    
    // establish the links
    for (KeyframeLink const& link : mLinks) {
      ImNodes::Link(link.linkId, link.outputPin, link.inputPin);
    }

    ImNodes::MiniMap(0.2f, ImNodesMiniMapLocation_BottomRight);
    ImNodes::EndNodeEditor();

    { // check for tooltip trigger
      bool const shiftHeld{ ImGui::IsKeyDown(ImGuiKey_LeftShift) || ImGui::IsKeyDown(ImGuiKey_RightShift) };
      IDType hoveredNodeId;
      if (shiftHeld && ImNodes::IsNodeHovered(&hoveredNodeId)) {
        // ignore tooltip for root
        if (hoveredNodeId != sRootId) {
          NodePreview(mNodes[hoveredNodeId]);
        }
      }
    }

    // check for link interaction
    IDType inPin, outPin, linkId;
    if (ImNodes::IsLinkCreated(&inPin, &outPin)) {
      //IGE_DBGLOGGER.LogInfo("Created");
      mLinks.emplace_back(++sLastLinkId, inPin, outPin);

      KeyframeNode::NodePtr& srcPin{ mPinIdToNode[outPin] },
        destPin{ mPinIdToNode[inPin - 1] };

      destPin->nextNodes.emplace_back(srcPin);
      srcPin->previous = destPin;
      UpdateChain(srcPin);
      modified = true;
    }
    if (ImNodes::IsLinkDestroyed(&linkId)) {
      //IGE_DBGLOGGER.LogInfo("Destroyed");
      auto result = std::find_if(mLinks.begin(), mLinks.end(), [linkId](KeyframeLink const& link) { return link.linkId == linkId; });
      if (result != mLinks.end()) {
        IDType inPin{ result->inputPin };

        std::erase_if(mPinIdToNode[result->outputPin]->nextNodes, [inPin](KeyframeNode::NodePtr const& node) { return inPin == node->inputPin; });
        mLinks.erase(result);
      }
      modified = true;
    }

    ImGui::End();

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
        }
      }
    }
  }

  void KeyframeEditor::DisplayRootNode(KeyframeNode::NodePtr const& root) {
    ImNodes::BeginNodeTitleBar();
    ImGui::TextUnformatted("Root Keyframe");
    ImNodes::EndNodeTitleBar();

    ImGui::Text("Current Transform");
    ImGui::BeginDisabled();

    if (ImGui::BeginTable("KeyframeNodePreviewTable", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingFixedFit)) {
      ImGui::TableSetupColumn("##Col1", ImGuiTableColumnFlags_WidthFixed, sLeftColWidth);
      ImGui::TableSetupColumn("##Col2", ImGuiTableColumnFlags_WidthFixed, sRightColWidth);
      CumulativeValues& vals{ root->cumulativeVal };

      NextRowTable("Position", sRightColWidth);
      NodeVec3Input("##Position", vals.pos);

      NextRowTable("Rotation", sRightColWidth);
      NodeVec3Input("##Rotation", vals.rot);

      NextRowTable("Scale", sRightColWidth);
      NodeVec3Input("##Scale", vals.scale);

      ImGui::EndTable();
    }

    ImGui::EndDisabled();

    if (root->outputPin != INVALID_ID) {
      ImNodes::BeginOutputAttribute(root->outputPin);
      ImNodes::EndOutputAttribute();
    }
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
            }
            break;
          }
        }

        ImGui::EndCombo();
      }

      if (node->data.type != Anim::KeyframeType::NONE) {
        NextRowTable("Target", sRightColWidth);
        glm::vec3& rawVal{ std::get<glm::vec3>(node->data.endValue) };
        if (NodeVec3Input("##Target Value", rawVal)) {
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

    if (ImGui::Button("New Keyframe")) {
      KeyframeNode::NodePtr newNode{
        std::make_shared<KeyframeNode>(Anim::Keyframe(), true, true)
      };

      mPinIdToNode.emplace(newNode->inputPin, newNode);
      mNodes.emplace(KeyframeNode::NextID(), std::move(newNode));
    }

    return modified;
  }

  void KeyframeEditor::UpdateChain(KeyframeNode::NodePtr const& node) {
    if (!node->previous) {
      node->cumulativeVal = {};
      return;
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

  void KeyframeEditor::Init() {
    Reset();

    KeyframeNode::NodePtr newNode{
      std::make_shared<KeyframeNode>(Anim::Keyframe(), false, true)
    };
    newNode->nodeName = "Root";

    mPinIdToNode.emplace(newNode->outputPin - 1, newNode);
    mNodes.emplace(KeyframeNode::NextID(), std::move(newNode));
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
      
    CloneKeyframeTree(GetRootNode(), dummy);

    return true;
  }

  void KeyframeEditor::SaveKeyframes(std::string const& filePath) const {
    // first, create the new keyframe tree
    Anim::AnimationData& animData{ IGE_ASSETMGR.GetAsset<IGE::Assets::AnimationAsset>(mSelectedAnim)->mAnimData };

    // create the root first
    KeyframeNode::NodePtr const& root{ GetRootNode() };
    animData.rootKeyframe = CreateRootKeyframe(root);

    // construct the rest of the tree
    for (KeyframeNode::NodePtr const& next : root->nextNodes) {
      KeyframeData const& data{ next->data };
      Anim::Node newKeyframe{ std::make_shared<Anim::Keyframe>(data.startValue, data.endValue, data.type, data.startTime, data.duration) };
      animData.rootKeyframe.nextNodes.emplace_back(newKeyframe);

      CreateOutputTree(newKeyframe, next);
    }

    // finally, save to file
    Serialization::Serializer::SerializeAnimationData(animData, filePath);
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
    }
    catch (Debug::ExceptionBase&) {
      IGE_DBGLOGGER.LogError("Error creating new animation: " + newFile);
    }
  }

  void KeyframeEditor::CloneKeyframeTree(KeyframeNode::NodePtr& dest, Anim::Node const& src) {
    // recursively construct the editor tree from the source tree
    for (Anim::Node const& next : src->nextNodes) {
      KeyframeNode::NodePtr newNode{
        std::make_shared<KeyframeNode>(*next)
      };
      newNode->previous = dest;
      dest->nextNodes.emplace_back(newNode);
      mNodes.emplace(newNode->NextID(), newNode);

      // create link
      mLinks.emplace_back(++sLastLinkId, newNode->inputPin, dest->outputPin);
      mPinIdToNode.emplace(newNode->inputPin, newNode);

      CloneKeyframeTree(newNode, next);
    }
  }

  void KeyframeEditor::CreateOutputTree(Anim::Node const& dest, KeyframeNode::NodePtr const& src) const {
    // recursively construct the new tree from the editor tree
    for (KeyframeNode::NodePtr const& next : src->nextNodes) {
      KeyframeData const& data{ src->data };
      Anim::Node newNode{
        std::make_shared<Anim::Keyframe>(data.startValue, data.endValue, data.type, data.startTime, data.duration)
      };
      dest->nextNodes.emplace_back(newNode);

      CreateOutputTree(newNode, next);
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
  bool NodeVec3Input(const char* fieldLabel, glm::vec3& val) {
    return ImGui::DragFloat3(fieldLabel, glm::value_ptr(val), 0.1f, -FLT_MAX, FLT_MAX, "%.2f");
  }

  void NextRowTable(const char* labelName, float elemWidth) {
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::Text(labelName);
    ImGui::TableSetColumnIndex(1);
    ImGui::SetNextItemWidth(elemWidth);
  }
}
