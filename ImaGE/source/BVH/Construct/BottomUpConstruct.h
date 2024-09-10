#pragma once
#include "BVHierarchy.h"

namespace BV
{

  class BottomUpConstruct : public BVHierarchy
  {
  public:
    BottomUpConstruct();

    void Construct(Node& node, ContainerType const& objects) override;
  private:
    static constexpr unsigned RANKING_CUTOFF = 10;  // the top N number of pairs to output

    struct NodePair
    {
      unsigned idx1, idx2;
      float value;

      NodePair() = default;
      NodePair(unsigned _idx1, unsigned _idx2, float val) : idx1{ _idx1 }, idx2{ _idx2 }, value{ val } {
        if (idx1 > idx2) { std::swap(idx1, idx2); }
      }

      // comparators for pq ordering
      inline bool operator<(NodePair const& rhs) const noexcept { return value < rhs.value; }
      inline bool operator>(NodePair const& rhs) const noexcept { return value > rhs.value; }
    };

    // based on each heuristic, add each pair's score up
    // then output the pair with the best score
    void MergeNodes(std::vector<Node>& nodes, Node& node1, Node& node2) const;
    void AddScores(std::map<std::pair<unsigned, unsigned>, int>& scores, std::vector<NodePair> const& results) const;

    // compare the distances (squared) between centers and find the nearest 2 BVs
    std::vector<NodePair> NearestNeighbor(std::vector<Node>& nodes) const;
    // compare the vol of the new BV encapsulating both BVs
    std::vector<NodePair> MinimumCombinedVolume(std::vector<Node>& nodes) const;
    // compare the increase in vol from both individual BVs vs the new BV encapsulating both
    std::vector<NodePair> MinRelativeVolIncrease(std::vector<Node>& nodes) const;
  };

} // namespace BV
