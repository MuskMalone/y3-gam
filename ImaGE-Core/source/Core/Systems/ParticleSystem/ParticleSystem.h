#pragma once
#include <Core/Systems/SystemManager/SystemManager.h>

namespace Systems{
	class ParticleSystem : public Systems::System {
	public:
		ParticleSystem(const char* name);
		void Update() override;
		void PausedUpdate() override;
	};
}