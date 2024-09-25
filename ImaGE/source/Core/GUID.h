#pragma once
#include <pch.h>
#include <functional>
#include <random>
namespace IGE {
	namespace Core {
		template <typename _tag>
		class GUID
		{
		private:
			using type = _tag;
		public:
			
			//GUID() : mID{ sUniformDistribution(sEng) } {}
			GUID(std::string const& str) {
				std::seed_seq seed(str.begin(), str.end());
				std::mt19937_64 rng(seed);
				mID = rng();
			}
			GUID(uint64_t guid) : mID{ guid } {}
			GUID(const GUID& other) : mID{ other.mID } {}

			operator uint64_t () { return mID; }
			operator const uint64_t() const { return mID; }

			bool operator==(const GUID& other) const { return mID == other.mID; } 
			bool operator<(const GUID& other) const { return mID < other.mID; }

		private:
			static std::mt19937_64 sEng;
			static std::uniform_int_distribution<uint64_t> sUniformDistribution;
			uint64_t mID;
		};
		//template <typename _tag>
		//std::mt19937_64 GUID<_tag>::sEng{ std::random_device{} };
		//template <typename _tag>
		//std::uniform_int_distribution<uint64_t> GUID<_tag>::sUniformDistribution;
	}
}