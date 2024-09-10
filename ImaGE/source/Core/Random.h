#pragma once
#include <random>
#include <type_traits>

namespace Random
{
  class RandGen
  {
  public:
    static int RandInt(int min = 1, int max = INT_MAX);
    static float RandFloat(float min = 1.f, float max = FLT_MAX);

    template <typename T>
    static std::vector<T> ShuffleVector(std::vector<T> const& vec)
    {
      auto cpy{ vec };
      std::shuffle(cpy.begin(), cpy.end(), m_engine);

      return cpy;
    }

  private:
    static std::mt19937 m_engine;
  };


} // namespace Random
