#pragma once
#include <GUI/GUIWindow.h>
#include <memory>
#include <vector>
#include <Animation/Keyframe.h>
#include <Asset/SmartPointer.h>

namespace ECS { class Entity; }
namespace Component { struct Transform; }

namespace GUI {
  class KeyframeEditor : public GUIWindow
  {
  public:
    KeyframeEditor(const char* windowName);
    ~KeyframeEditor();

    void Run() override;

  private:
    using IDType = int;
    inline static constexpr IDType sRootId = 0;
    inline static constexpr IDType INVALID_ID = -1;
    inline static IDType sLastLinkId = -1;
    inline static constexpr float sNodeWidth = 150.f;

    struct CumulativeValues {
      CumulativeValues() : pos{}, rot{}, scale{ 1.f, 1.f, 1.f } {}
      CumulativeValues(Component::Transform const& trans);

      glm::vec3 pos, rot, scale;
    };

    struct KeyframeNode {
      using NodePtr = std::shared_ptr<KeyframeNode>;

      KeyframeNode(Anim::Keyframe const& keyframeData, bool hasInputPin = true, bool hasOutputPin = true);

      inline static IDType sLastNodeId = sRootId, sLastPinId = -1;

      static void ResetIDs() {
        sLastNodeId = sRootId;
        sLastPinId = -1;
      }

      inline static IDType NextID() { return sLastNodeId++; }

      CumulativeValues cumulativeVal;
      std::string nodeName;
      std::vector<NodePtr> nextNodes;
      NodePtr previous;
      Anim::Keyframe data;
      IDType inputPin, outputPin;
      //IDType nodeId;
    };
    
    struct KeyframeLink {
      KeyframeLink(IDType id, IDType input, IDType output) :
        linkId{ id }, inputPin{ input }, outputPin{ output } {}

      IDType linkId, inputPin, outputPin;
    };

    void Init(Component::Transform const& trans);
    void UpdateChain(KeyframeNode::NodePtr const& parentNode);
    void Reset();
    bool LoadKeyframes(ECS::Entity entity);

    KeyframeNode::NodePtr const& GetRootNode() const { return mNodes.at(sRootId); }

    void NodePreview(KeyframeNode::NodePtr const& node);
    void DisplayRootNode(KeyframeNode::NodePtr const& root, Component::Transform const& trans);
    bool KeyframeNodeBody(KeyframeNode::NodePtr const& node);
    bool NodesToolbar();

    std::unordered_map<IDType, KeyframeNode::NodePtr> mPinIdToNode;
    std::unordered_map<IDType, KeyframeNode::NodePtr> mNodes;
    std::vector<KeyframeLink> mLinks;
    IGE::Assets::GUID mSelectedAnim;
  };
}