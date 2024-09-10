#pragma once
#include "BVHierarchy.h"

namespace BV
{

  class TopDownConstruct : public BVHierarchy
  {
  public:

    enum ChosenAxis
    {
      X_AXIS = 0,
      Y_AXIS,
      Z_AXIS
    };

    ChosenAxis m_chosenAxis;

    TopDownConstruct();

    void Construct(Node& node, ContainerType const& objects) override;

  private:
    using SplitPlaneFunc = ContainerType::iterator(TopDownConstruct::*)(ContainerType::iterator, unsigned);

    static constexpr unsigned MIN_OBJECTS_AT_LEAF = 1;
    static constexpr unsigned MAX_HEIGHT = 7;
    static constexpr unsigned EVEN_SPLITS = 10;

    static SplitPlaneFunc m_methods[SplitMethod::NUM_METHODS];

    struct Extent
    {
      size_t index;
      float coord;
      bool max;

      Extent(float _coord, size_t _index, bool _max) : index{ _index }, coord{ _coord }, max{ _max } {}

      inline bool operator<(Extent const& rhs) const noexcept { return coord < rhs.coord; }
      inline bool operator>(Extent const& rhs) const noexcept { return coord > rhs.coord; }
    };

    void ConstructTopDownTree(Node& node,
      ContainerType::iterator object, unsigned objectCount, unsigned level = 0);
    ContainerType::iterator PartitionObjects(ContainerType::iterator object, unsigned numObjects);
    // get the plane with the largest spread to minimize
    // total vol. occupied by child nodes
    void DetermineAxis(ContainerType::iterator object, unsigned numObjects);

    // partition BVs based on the median of centers
    ContainerType::iterator MedianOfCenters(ContainerType::iterator object, unsigned numObjects);
    // partition BVs based on the median of min and max extents
    ContainerType::iterator MedianOfExtents(ContainerType::iterator object, unsigned numObjects);
    // split plane into multiple even sections, checking each one for the best split
    ContainerType::iterator KEvenSplits(ContainerType::iterator object, unsigned numObjects);
  };

} // namespace BV
