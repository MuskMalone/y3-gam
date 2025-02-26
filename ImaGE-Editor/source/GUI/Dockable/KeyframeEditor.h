#pragma once
#include <GUI/GUIWindow.h>
#include <memory>
#include <vector>
#include <unordered_map>
#include <ImGui/imgui.h>
#include <Animation/Keyframe.h>
#include <Asset/SmartPointer.h>
#include <Events/EventCallback.h>

namespace Anim { struct RootKeyframe; }
namespace Component { struct Transform; }

namespace GUI {
  class KeyframeEditor : public GUIWindow
  {
  public:
    using IDType = int;
    using NodePosMap = std::unordered_map<IDType, ImVec2>;
    struct NodePositions {
      NodePosMap posMap;
    };

  public:
    KeyframeEditor(const char* windowName);
    ~KeyframeEditor();

    void Run() override;

  private:
    inline static constexpr IDType sRootId = 0;
    inline static constexpr IDType INVALID_ID = -1;
    inline static IDType sLastLinkId = -1;
    inline static constexpr float sNodeWidth = 150.f;
    inline static constexpr unsigned sTimeUntilSave = 1000; // in ms
    inline static constexpr const char* sMetadataKey = "KeyframeEditorData";  // path to editor file
    inline static constexpr const char* sNodeOptionsPopupLabel = "NodeOptions";
    inline static constexpr const char* sOptionsPopupLabel = "Options";
    static const std::string sEditorFilePath;
    
    struct KeyframeData {
      using ValueType = Anim::Keyframe::ValueType;

      KeyframeData() : endValue{}, type{ Anim::KeyframeType::NONE }, startTime{}, duration{} {}
      KeyframeData(Anim::Keyframe const& keyframe) :
        endValue{ keyframe.endValue }, type{ keyframe.type },
        startTime{ keyframe.startTime }, duration{ keyframe.duration } {}

      template <typename T>
      T GetInterpolatedValue(T const& startVal, float endTime) const { 
        return glm::mix(startVal, std::get<T>(endValue), (endTime - startTime) / duration);
      }

      inline float GetEndTime() const noexcept { return startTime + duration; }

      ValueType endValue;
      Anim::KeyframeType type;
      float startTime, duration;
    };

    struct KeyframeNode {
      using NodePtr = std::shared_ptr<KeyframeNode>;

      KeyframeNode(IDType nodeId, bool hasInputPin = true, bool hasOutputPin = true);
      KeyframeNode(IDType nodeId, Anim::Keyframe const& keyframeData, bool hasInputPin = true, bool hasOutputPin = true);
      
      inline static IDType sLastNodeId = sRootId, sLastPinId = -1;

      inline static IDType NextID() { return ++sLastNodeId; }
      static void ResetIDs() {
        sLastNodeId = sRootId;
        sLastPinId = -1;
      }

      void RemoveLinkedNode(IDType id);

      KeyframeData data;
      std::string nodeName;
      std::vector<NodePtr> nextNodes;
      NodePtr previous;
      IDType inputPin, outputPin;
      IDType id;
    };

    struct RootKeyframeNode : public KeyframeNode {
      RootKeyframeNode() : KeyframeNode(sRootId, {}, false, true),
        startPos{}, startRot{}, startScale{ 1.f, 1.f, 1.f } {
        nodeName = "Root";
      }

      Anim::RootKeyframe ToRootKeyframe() const;

      glm::vec3 startPos, startRot, startScale;
    };
    
    struct KeyframeLink {
      KeyframeLink(IDType id, IDType input, IDType output) :
        linkId{ id }, inputPin{ input }, outputPin{ output } {}

      IDType linkId, inputPin, outputPin;
    };

    void Init();
    void InitRoot(Anim::RootKeyframe const& root);
    void Reset();

    void UpdateStartTimesInChain(KeyframeNode::NodePtr& updatedNode);
    void GetCumulativeValue(KeyframeNode::NodePtr const& node, Anim::KeyframeType type) const;
    void GetCumulativeValues(KeyframeNode::NodePtr const& node) const;
    void GetCumulativeValueR(KeyframeNode::NodePtr const& node, float startTime, float latestUpdate, Anim::KeyframeType type) const;
    void GetCumulativeValuesR(KeyframeNode::NodePtr const& node, float startTime, std::vector<float>& latestUpdate) const;

    void CreateEditorFile(std::string const& animFile) const;
    bool LoadKeyframes(IGE::Assets::GUID guid);
    void SaveKeyframes(std::string const& filePath) const;
    void CloneKeyframeTree(KeyframeNode::NodePtr& dest, Anim::Node const& src, std::unordered_map<IDType, IDType>& posMap);
    void CreateOutputTree(Anim::Node& dest, KeyframeNode::NodePtr const& src, NodePosMap& nodePosMap) const;

    void NodePreview(KeyframeNode::NodePtr const& node);
    void PreviewKeyframe(KeyframeNode::NodePtr const& node, Component::Transform& transform) const;
    bool DisplayRootNode();
    bool KeyframeNodeBody(KeyframeNode::NodePtr& node);

    bool NodeOptionsMenu();
    bool OptionsMenu();
    bool NodesToolbar();

    Anim::RootKeyframe CreateRootKeyframe() const;
    void NewAnimation();
    KeyframeNode::NodePtr NewNode();

    EVENT_CALLBACK_DECL(OnAnimationEdit);

    std::unordered_map<IDType, KeyframeNode::NodePtr> mPinIdToNode; // indexed by inPin
    std::unordered_map<IDType, KeyframeNode::NodePtr> mNodes;
    std::vector<KeyframeLink> mLinks;
    std::shared_ptr<RootKeyframeNode> mRoot;
    IGE::Assets::GUID mSelectedAnim;
  };
}