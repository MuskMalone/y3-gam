#pragma once
#include <pch.h>
#include <functional>
#include <random>
#include <rttr/registration_friend>

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
			struct Seed {
			private:
				inline uint64_t GetCurrentTimeInNanoseconds() {
					return std::chrono::time_point_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now()).time_since_epoch().count();
				}
			public:
				Seed() : s{ GetCurrentTimeInNanoseconds() } {}
				uint64_t s;
			};
			// guid will only be null if mID is 0
			// cast it to uint64_t and check if its != 0 for validity
			GUID() : /*mSeed{ "INVALID" },*/ mID{ } {} 
			GUID(Seed seed) {
				std::mt19937_64 rng(seed.s);
				mID = rng();
			}
			GUID(std::string const& str) /*mSeed{str} */{
				std::seed_seq seed(str.begin(), str.end());
				std::mt19937_64 rng(seed);
				mID = rng();
			}
			GUID(uint64_t guid) : /*mSeed{"INVALID"}, */mID{guid} {}
			GUID(const GUID& other) : 
				//mSeed{ other.mSeed}, 
				mID { other.mID } {}

			operator uint64_t () { return mID; }
			operator const uint64_t() const { return mID; }

			bool operator==(const GUID& other) const { return mID == other.mID; } 
			bool operator<(const GUID& other) const { return mID < other.mID; }
			//std::string const& GetSeed() const noexcept { return mSeed; }
			bool IsValid() const noexcept { return mID != 0; }
			void Reset() { mID = 0; }
		private:
			//static std::mt19937_64 sEng;
			//static std::uniform_int_distribution<uint64_t> sUniformDistribution;
			//std::string mSeed;
			uint64_t mID;

			RTTR_REGISTRATION_FRIEND
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