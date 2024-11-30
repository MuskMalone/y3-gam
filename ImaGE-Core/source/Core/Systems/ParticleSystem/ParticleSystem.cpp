#include <pch.h>
#include "ParticleSystem.h"
#include "Graphics/PostProcessing/ParticleManager.h"
#include "Graphics/Shader.h"
#include "FrameRateController/FrameRateController.h"
namespace Systems {
	ParticleSystem::ParticleSystem(const char* name) : Systems::System{ name }
	{}
	void ParticleSystem::Update()
	{
		std::cout << "FROM PARTICLE SYSTEM\n";
		//Graphics::ParticleManager::GetInstance().Bind();
		float dt{ Performance::FrameRateController::GetInstance().GetDeltaTime() };
		auto const& emitterStepShader{ Graphics::ShaderLibrary::Get("EmitterStep") };
		auto const& particleStepShader{ Graphics::ShaderLibrary::Get("ParticleStep") };
		emitterStepShader->Use();
		
		emitterStepShader->SetUniform("DT", dt);
		emitterStepShader->SetUniform("bufferMaxCount", MAX_BUFFER);
		//1000 is the number of elements per grp
		glDispatchCompute(MAX_BUFFER / WORK_GROUP, 1, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
		emitterStepShader->Unuse();

		particleStepShader->Use();
		particleStepShader->SetUniform("DT", dt);
		//mParticleShader->SetUniform("bufferMaxCount", MAX_BUFFER);
		//1000 is the number of elements per grp
		glDispatchCompute(MAX_BUFFER / WORK_GROUP, 1, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
		particleStepShader->Unuse();
		//Graphics::ParticleManager::GetInstance().Unbind();
	}
	void ParticleSystem::PausedUpdate()
	{
		Update();
		//to update emitter stuff
	}
}