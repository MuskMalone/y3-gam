#include <pch.h>
#include "SmartPointer.h"

namespace IGE{
	namespace Assets {

		static std::unordered_set<void*> s_LiveReferences;
#ifdef IGE_MULTITHREAD
		static std::mutex s_LiveReferenceMutex;
#endif
		namespace RefUtils {

			void AddToLiveReferences(void* instance)
			{
#ifdef IGE_MULTITHREAD
				std::scoped_lock<std::mutex> lock(s_LiveReferenceMutex);
#endif
				s_LiveReferences.insert(instance);
			}

			void RemoveFromLiveReferences(void* instance)
			{
#ifdef IGE_MULTITHREAD
				std::scoped_lock<std::mutex> lock(s_LiveReferenceMutex);
#endif
				s_LiveReferences.erase(instance);
			}

			bool IsLive(void* instance)
			{
				return s_LiveReferences.find(instance) != s_LiveReferences.end();
			}
		}
	}
}