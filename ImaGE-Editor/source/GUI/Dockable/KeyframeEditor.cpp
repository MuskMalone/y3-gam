#include <pch.h>
#include "KeyframeEditor.h"
#include <ImNodes/imnodes.h>
#include <rttr/variant.h>
#include <GUI/Helpers/ImGuiHelpers.h>
#include <Core/Entity.h>
#include <Core/Components/Transform.h>
#include <Core/Components/Animation.h>
#include <GUI/GUIVault.h>
#include <Asset/IGEAssets.h>

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
  KeyframeEditor::CumulativeValues::CumulativeValues(Component::Transform const& trans) :
    pos{ trans.position }, rot{ trans.eulerAngles }, scale{ trans.scale } {}

  KeyframeEditor::KeyframeNode::KeyframeNode(Anim::Keyframe const& keyframeData, bool hasInputPin, bool hasOutputPin) :
    cumulativeVal{}, nodeName{ sKeyframeTypeStr[static_cast<int>(keyframeData.type)] }, nextNodes{},
    previous{}, data{ keyframeData }//, nodeId{ sLastNodeId++ } 
  {
    inputPin = hasInputPin ? ++sLastPinId : INVALID_ID;
    outputPin = hasOutputPin ? ++sLastPinId : INVALID_ID;

    //SUBSCRIBE_CLASS_FUNC();
  }

  KeyframeEditor::KeyframeEditor(const char* windowName) : GUIWindow(windowName),
    mPinIdToNode{}, mNodes{}, mLinks{} {
    // the context shall live and die with this window
    ImNodes::CreateContext();
    ImNodesIO& io{ ImNodes::GetIO() };
    io.LinkDetachWithModifierClick.Modifier = &ImGui::GetIO().KeyCtrl;
    //ImNodes::PushAttributeFlag(ImNodesAttributeFlags_EnableLinkDetachWithDragClick);
  }

  void KeyframeEditor::Run() {
    ImGui::Begin(mWindowName.c_str());

    static ECS::Entity prevEntity{};
    ECS::Entity currEntity{ GUIVault::GetSelectedEntity() };
    if (!currEntity || !currEntity.HasComponent<Component::Animation>()) {
      ImGui::Text("Add an Animation component to get started!");
      ImGui::End();
      return;
    }

    Component::Transform const& entityTransform{ currEntity.GetComponent<Component::Transform>() };

    if (currEntity != prevEntity) {
      Init(entityTransform);
      LoadKeyframes(currEntity);
    }

    if (NodesToolbar()) {
      
    }

    ImNodes::BeginNodeEditor();

    for (auto&[id, node] : mNodes) {
      ImNodes::BeginNode(id);

      if (id == sRootId) {
        DisplayRootNode(node, entityTransform);
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
      IGE_DBGLOGGER.LogInfo("Created");
      mLinks.emplace_back(++sLastLinkId, inPin, outPin);

      KeyframeNode::NodePtr& srcPin{ mPinIdToNode[outPin] },
        destPin{ mPinIdToNode[inPin - 1] };

      destPin->nextNodes.emplace_back(srcPin);
      srcPin->previous = destPin;
      UpdateChain(srcPin);
    }
    if (ImNodes::IsLinkDestroyed(&linkId)) {
      IGE_DBGLOGGER.LogInfo("Destroyed");
      auto result = std::find_if(mLinks.begin(), mLinks.end(), [linkId](KeyframeLink const& link) { return link.linkId == linkId; });
      if (result != mLinks.end()) {
        IDType inPin{ result->inputPin };

        std::erase_if(mPinIdToNode[result->outputPin]->nextNodes, [inPin](KeyframeNode::NodePtr const& node) { return inPin == node->inputPin; });
        mLinks.erase(result);
      }
    }

    ImGui::End();

    prevEntity = currEntity;
  }

  void KeyframeEditor::DisplayRootNode(KeyframeNode::NodePtr const& root, Component::Transform const& trans) {
    ImNodes::BeginNodeTitleBar();
    ImGui::TextUnformatted("Root Keyframe");
    ImNodes::EndNodeTitleBar();

    ImGui::Text("Current Transform");
    ImGui::BeginDisabled();

    if (ImGui::BeginTable("KeyframeNodePreviewTable", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingFixedFit)) {
      ImGui::TableSetupColumn("##Col1", ImGuiTableColumnFlags_WidthFixed, sLeftColWidth);
      ImGui::TableSetupColumn("##Col2", ImGuiTableColumnFlags_WidthFixed, sRightColWidth);
      glm::vec3 pos{ trans.position }, rot{ trans.eulerAngles }, scale{ trans.scale };

      NextRowTable("Position", sRightColWidth);
      NodeVec3Input("##Position", pos);

      NextRowTable("Rotation", sRightColWidth);
      NodeVec3Input("##Rotation", rot);

      NextRowTable("Scale", sRightColWidth);
      NodeVec3Input("##Scale", scale);

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

    Anim::Keyframe const& prevData{ node->previous->data };
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

  void KeyframeEditor::Init(Component::Transform const& trans) {
    Reset();

    KeyframeNode::NodePtr newNode{
      std::make_shared<KeyframeNode>(Anim::Keyframe(), false, true)
    };
    newNode->nodeName = "Root";
    newNode->cumulativeVal = { trans };

    mPinIdToNode.emplace(newNode->outputPin - 1, newNode);
    mNodes.emplace(KeyframeNode::NextID(), std::move(newNode));
  }

  bool KeyframeEditor::LoadKeyframes(ECS::Entity entity) {
    Component::Animation const& animation{ entity.GetComponent<Component::Animation>() };

    if (animation.animations.empty()) { return false; }

    IGE::Assets::AssetManager& am{ IGE_ASSETMGR };
    try {
      am.LoadRef<IGE::Assets::AnimationAsset>(*animation.animations.begin());
    }
    catch (Debug::ExceptionBase&) {
      IGE_DBGLOGGER.LogError("[AnimationSystem] Unable to get animation " +
        std::to_string(static_cast<uint64_t>(*animation.animations.begin())) + " of Entity " + entity.GetTag());
      return false;
    }

    // default to first animation in the list (or map)
    auto const& [name, keyframes]{ am.GetAsset<IGE::Assets::AnimationAsset>(*animation.animations.begin())->mAnimData };
    mNodes.reserve(keyframes.size());
    std::unordered_map<float, KeyframeNode::NodePtr> endTimeToNode;
    endTimeToNode.emplace(0.f, GetRootNode());

    for (Anim::Keyframe const& keyframe : keyframes) {
      KeyframeNode::NodePtr newNode{
        std::make_shared<KeyframeNode>(keyframe)
      };

      // store a local map of <EndTime, NodePtr>
      endTimeToNode[keyframe.GetEndTime()] = newNode;
      // taking advantage of the sorted list,
      // if we find a node that starts on the same existing EndTime,
      // add it as that node's next ptr
      if (endTimeToNode.contains(keyframe.startTime)) {
        KeyframeNode::NodePtr& prev{ endTimeToNode[keyframe.startTime] };
        prev->nextNodes.emplace_back(newNode);
        newNode->previous = prev;

        // rmb to add the link
        mLinks.emplace_back(++sLastLinkId, newNode->inputPin, prev->outputPin);
      }

      mPinIdToNode.emplace(newNode->outputPin - 1, newNode);
      mNodes.emplace(KeyframeNode::NextID(), std::move(newNode));
    }

    return true;
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
    return ImGui::DragFloat3("##Target Value", glm::value_ptr(val), 0.1f, -FLT_MAX, FLT_MAX, "%.2f");
  }

  void NextRowTable(const char* labelName, float elemWidth) {
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::Text(labelName);
    ImGui::TableSetColumnIndex(1);
    ImGui::SetNextItemWidth(elemWidth);
  }
}
