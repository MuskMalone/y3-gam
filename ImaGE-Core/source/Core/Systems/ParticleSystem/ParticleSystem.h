#pragma once
#include <Core/Systems/SystemManager/SystemManager.h>
#include <Events/EventCallback.h>

namespace Systems{
	class ParticleSystem : public Systems::System {
	public:
		ParticleSystem(const char* name);
		void Update() override;
		void PausedUpdate() override;
		void Debug();
	private:
		EVENT_CALLBACK_DECL(HandleRemoveComponent);
		EVENT_CALLBACK_DECL(HandleRemoveEntity);
	};
}