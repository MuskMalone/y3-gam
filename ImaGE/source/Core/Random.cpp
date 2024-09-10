#include <pch.h>
#include "Random.h"

namespace Random
{
  std::mt19937 RandGen::m_engine{ std::random_device{}() };


  int RandGen::RandInt(int min, int max)
  {
    std::uniform_int_distribution<int> dist{ min, max };
    return dist(m_engine);
  }

  float RandGen::RandFloat(float min, float max)
  {
    std::uniform_real_distribution<float> dist(min, std::nextafterf(max, std::numeric_limits<float>::max()));
    return dist(m_engine);
  }

} // namespace Random
