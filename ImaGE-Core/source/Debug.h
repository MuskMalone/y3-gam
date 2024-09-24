#pragma once
#include <iostream>
#include <iomanip>

// operator overloads for debug mode
// << operator overload for printing Vectors to ostream
template <unsigned N, typename T>
std::ostream& operator<<(std::ostream& os, glm::vec<N, T> const& vec)
{
  os << "(";
  for (unsigned i{}; i < N; ++i)
  {
    os << vec[i] << ((i == N - 1) ? ")" : ",");
  }
  return os;
}

// << operator overload for printing Matrices to ostream
template <unsigned N, unsigned M, typename T>
std::ostream& operator<<(std::ostream& os, glm::mat<N, M, T> const& mat)
{
  os << std::setprecision(2) << std::fixed;
  for (unsigned i{}; i < N; ++i)
  {
    os << "| ";
    for (unsigned j{}; j < M; ++j)
    {
      os << mat[i][j] << (j == M - 1 ? " |\n" : " ");
    }
  }
  return os;
}

// allow printing of vector contents to ostream
template <typename T>
std::ostream& operator<<(std::ostream& os, std::vector<T> const& vec)
{
  os << "{ ";
  for (unsigned i{}; i < vec.size(); ++i)
  {
    os << vec[i] << (i + 1 == vec.size() ? " }" : ", ");
  }
  return os;
}
