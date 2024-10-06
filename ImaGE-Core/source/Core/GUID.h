#pragma once
#include <pch.h>
#include <functional>
#include <random>
namespace IGE {
	namespace Core {
		inline uint32_t Fnv1a32(char const* s, size_t count)
		{
			return ((count ? Fnv1a32(s, count - 1) : 2166136261u) ^ s[count]) * 16777619u; // NOLINT (hicpp-signed-bitwise)
		}
		template <typename _tag>
		class GUID
		{
		private:
			using type = _tag;
		public:
			GUID() : mSeed{ "INVALID" }, mID{ } {}
			GUID(std::string const& str) : mSeed{str} {
				std::seed_seq seed(str.begin(), str.end());
				std::mt19937_64 rng(seed);
				mID = rng();
			}
			GUID(uint64_t guid) : mSeed{"INVALID"}, mID{guid} {}
			GUID(const GUID& other) : 
				mSeed{ other.mSeed}, 
				mID { other.mID } {}

			operator uint64_t () { return mID; }
			operator const uint64_t() const { return mID; }

			bool operator==(const GUID& other) const { return mID == other.mID; } 
			bool operator<(const GUID& other) const { return mID < other.mID; }
			std::string const& GetSeed() const noexcept { return mSeed; }
		private:
			//static std::mt19937_64 sEng;
			//static std::uniform_int_distribution<uint64_t> sUniformDistribution;
			std::string mSeed;
			uint64_t mID;
		};
		//template <typename _tag>
		//std::mt19937_64 GUID<_tag>::sEng{ };
		//template <typename _tag>
		//std::uniform_int_distribution<uint64_t> GUID<_tag>::sUniformDistribution;
	}
}
namespace std {
	template <typename _tag>
	struct hash<IGE::Core::GUID<_tag>> {
		std::size_t operator()(const IGE::Core::GUID<_tag>& guid) const noexcept {
			return static_cast<uint64_t>(guid);  // Hash based on the mID
		}
	};
}