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
    static const std::string sEditorFilePath;

    struct CumulativeValues {
      CumulativeValues() : pos{}, rot{}, scale{ 1.f, 1.f, 1.f } {}

      glm::vec3 pos, rot, scale;
    };
    
    struct KeyframeData {
      using ValueType = Anim::Keyframe::ValueType;

      KeyframeData() : startValue{}, endValue{}, type{ Anim::KeyframeType::NONE }, startTime{}, duration{} {}
      KeyframeData(Anim::Keyframe const& keyframe) :
        startValue{ keyframe.startValue }, endValue{ keyframe.endValue },
        type{ keyframe.type }, startTime{ keyframe.startTime }, duration{ keyframe.duration } {}

      template <typename T>
      T GetNormalizedValue() const { return std::get<T>(endValue) - std::get<T>(startValue); }

      inline float GetEndTime() const noexcept { return startTime + duration; }

      ValueType startValue, endValue;
      Anim::KeyframeType type;
      float startTime, duration;
    };

    struct KeyframeNode {
      using NodePtr = std::shared_ptr<KeyframeNode>;

      KeyframeNode(IDType nodeId, Anim::Keyframe const& keyframeData, bool hasInputPin = true, bool hasOutputPin = true);

      inline static IDType sLastNodeId = sRootId, sLastPinId = -1;

      inline static IDType NextID() { return ++sLastNodeId; }
      static void ResetIDs() {
        sLastNodeId = sRootId;
        sLastPinId = -1;
      }

      void RemoveLinkedNode(IDType id);

      KeyframeData data;
      CumulativeValues cumulativeVal;
      std::string nodeName;
      std::vector<NodePtr> nextNodes;
      NodePtr previous;
      IDType inputPin, outputPin;
      IDType id;
    };
    
    struct KeyframeLink {
      KeyframeLink(IDType id, IDType input, IDType output) :
        linkId{ id }, inputPin{ input }, outputPin{ output } {}

      IDType linkId, inputPin, outputPin;
    };

    void Init();
    void InitRoot(Anim::RootKeyframe const& root);
    Anim::RootKeyframe CreateRootKeyframe(KeyframeNode::NodePtr const& root) const;
    void CloneKeyframeTree(KeyframeNode::NodePtr& dest, Anim::Node const& src, NodePosMap const& posMap);
    void CreateOutputTree(Anim::Node& dest, KeyframeNode::NodePtr const& src, NodePosMap& nodePosMap) const;
    void UpdateChainR(KeyframeNode::NodePtr const& parentNode);
    void UpdateChain(KeyframeNode::NodePtr const& parentNode);
    void Reset();
    void CreateEditorFile(std::string const& animFile) const;
    bool LoadKeyframes(IGE::Assets::GUID guid);
    void SaveKeyframes(std::string const& filePath) const;

    KeyframeNode::NodePtr const& GetRootNode() const { return mNodes.at(sRootId); }
    KeyframeNode::NodePtr& GetRootNode() { return mNodes.at(sRootId); }

    void NodePreview(KeyframeNode::NodePtr const& node);
    bool DisplayRootNode(KeyframeNode::NodePtr const& root);
    bool KeyframeNodeBody(KeyframeNode::NodePtr const& node);
    bool NodeOptionsMenu();
    bool NodesToolbar();
    void NewAnimation();

    EVENT_CALLBACK_DECL(OnAnimationEdit);

    std::unordered_map<IDType, KeyframeNode::NodePtr> mPinIdToNode; // indexed by inPin
    std::unordered_map<IDType, KeyframeNode::NodePtr> mNodes;
    std::vector<KeyframeLink> mLinks;
    IGE::Assets::GUID mSelectedAnim;
  };
}