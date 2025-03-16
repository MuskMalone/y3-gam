#pragma once
namespace Component
{
    struct Bloom
    {
        Bloom() {}
        inline void Clear() noexcept { threshold = 1.f; intensity = 1.f; range = 10.f; }
        float threshold{ 1.f };
        float intensity{ 1.f };
        float range{ 10.f };
    };

} // namespace Component