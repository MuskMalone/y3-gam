#pragma once
#include <string>
#include <cstdio>

namespace Serialization {
  class FILEWrapper {
  public:
    FILEWrapper(std::string const& filePath, const char* flags = "w") : mFile{ std::fopen(filePath.c_str(), flags) } {}
    ~FILEWrapper() { if (mFile) { Close(); } }

    operator bool() const { return mFile; }

    inline FILE* GetFILE() noexcept { return mFile; }
    inline void Close() { std::fclose(mFile); }

  private:
    FILE* mFile;
  };
}
