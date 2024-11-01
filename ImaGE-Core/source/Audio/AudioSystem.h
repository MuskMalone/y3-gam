#pragma once
#include <Core/Systems/SystemManager/SystemManager.h>

namespace IGE {
	namespace Audio {
		class AudioSystem : public Systems::System {
		public:
			AudioSystem(const char* name);
			void Update() override;
		};
	}
}