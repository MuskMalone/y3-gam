#pragma once
#include <Core/Systems/SystemManager/SystemManager.h>
#include <Events/EventCallback.h>

namespace Systems{
	class ParticleSystem : public Systems::System {
	public:
		ParticleSystem(const char* name);
		void Update() override;
		void PausedUpdate() override;

	private:
		EVENT_CALLBACK_DECL(HandleRemoveComponent);
		EVENT_CALLBACK_DECL(HandleRemoveEntity);
	};
}