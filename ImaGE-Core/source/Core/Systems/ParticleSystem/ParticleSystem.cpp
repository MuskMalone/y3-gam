#include <pch.h>
#include "ParticleSystem.h"
#include "Graphics/PostProcessing/ParticleManager.h"
namespace Systems {
	ParticleSystem::ParticleSystem(const char* name) : Systems::System{ name }
	{}
	void ParticleSystem::Update()
	{

	}
	void ParticleSystem::PausedUpdate()
	{
	}
}