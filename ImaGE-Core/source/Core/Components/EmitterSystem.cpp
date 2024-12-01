#include <pch.h>
#include "EmitterSystem.h"

namespace Component {
	void EmitterSystem::Clear() noexcept
	{
		Graphics::ParticleManager::GetInstance().MultiEmitterAction(emitters, -1);
		emitters.clear();
	}
	void EmitterSystem::AddEmitter()
	{
		emitters.emplace_back(Graphics::EmitterInstance{});
		Graphics::ParticleManager::GetInstance().EmitterAction(emitters.back(), 1);
	}
	void EmitterSystem::RemoveEmitter(uint64_t idx)
	{
		if (idx >= emitters.size()) return;
		Graphics::ParticleManager::GetInstance().EmitterAction(emitters[idx], -1);
	}
	std::vector<Graphics::EmitterInstance> const& EmitterSystem::GetEmitters()
	{
		return emitters;
	}
}