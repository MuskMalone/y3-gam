#include <pch.h>
#include "ParticleSystem.h"
#include "Graphics/PostProcessing/ParticleManager.h"
#include "Graphics/Shader.h"
#include "FrameRateController/FrameRateController.h"
#include "Events/EventManager.h"
#include "Core/Components/Components.h"

namespace Systems {
	ParticleSystem::ParticleSystem(const char* name) : Systems::System{ name }
	{
		SUBSCRIBE_CLASS_FUNC(Events::RemoveComponentEvent, &ParticleSystem::HandleRemoveComponent, this);
		SUBSCRIBE_CLASS_FUNC(Events::RemoveEntityEvent, &ParticleSystem::HandleRemoveEntity, this);
	}
	void ParticleSystem::Update()
	{
		auto ptclsystem{ ECS::EntityManager::GetInstance().GetAllEntitiesWithComponents<Component::EmitterSystem, Component::Transform>() };
		for (auto entity : ptclsystem) {
			ECS::Entity e{ entity };
			//std::cout << e.GetComponent<Component::Tag>().tag << "this is the tag in component" << std::endl;
			//if (!e.IsActive()) { continue; }

			// this is not a ref. i have to update all the transforms as they are offsets of the main transform
			std::vector<Graphics::EmitterInstance> vecProxy{ e.GetComponent<Component::EmitterSystem>().emitters };
			auto const& pos{ e.GetComponent<Component::Transform>().worldPos };
			//update the transforms
			for (int i{}; i < vecProxy.size(); ++i) {
				auto& emitproxy{ vecProxy[i] };
				if (e.IsActive()) {
					emitproxy.active = true;
				}
				else {
					emitproxy.active = false;
					continue;
				}
				for (int j{}; j < emitproxy.vCount; ++j)
					emitproxy.vertices[j][0] += pos.x, emitproxy.vertices[j][1] += pos.y, emitproxy.vertices[j ][2] += pos.z;
				Graphics::ParticleManager::GetInstance().EmitterAction(emitproxy, 0);
				//e.GetComponent<Component::EmitterSystem>().emitters[i].active = false;
			}
			//updates the emitters
			//Graphics::ParticleManager::GetInstance().MultiEmitterAction(vecProxy, 0);
		}
		//Graphics::ParticleManager::GetInstance().Bind();
		float dt{ Performance::FrameRateController::GetInstance().GetDeltaTime() };
		auto const& emitterStepShader{ Graphics::ShaderLibrary::Get("EmitterStep") };
		auto const& particleStepShader{ Graphics::ShaderLibrary::Get("ParticleStep") };
		emitterStepShader->Use();
		
		emitterStepShader->SetUniform("DT", dt);
		emitterStepShader->SetUniform("bufferMaxCount", MAX_BUFFER);
		//1000 is the number of elements per grp
		glDispatchCompute(MAX_BUFFER / WORK_GROUP, 1, 1);
		glMemoryBarrier(GL_ALL_BARRIER_BITS);
		emitterStepShader->Unuse();

		particleStepShader->Use();
		particleStepShader->SetUniform("DT", dt);
		//mParticleShader->SetUniform("bufferMaxCount", MAX_BUFFER);
		//1000 is the number of elements per grp
		glDispatchCompute(MAX_BUFFER / WORK_GROUP, 1, 1);
		glMemoryBarrier(GL_ALL_BARRIER_BITS);
		particleStepShader->Unuse();

		Graphics::ParticleManager::GetInstance().DebugSSBO();
		//Graphics::ParticleManager::GetInstance().Unbind();
	}

	void ParticleSystem::PausedUpdate()
	{
		Update();
		//to update emitter stuff
	}


	EVENT_CALLBACK_DEF(ParticleSystem, HandleRemoveComponent) {
		auto e{ CAST_TO_EVENT(Events::RemoveComponentEvent) };
		ECS::Entity entity{ const_cast<ECS::Entity&>(e->mEntity) };
		
		Graphics::ParticleManager::GetInstance().MultiEmitterAction(entity.GetComponent<Component::EmitterSystem>().emitters, -1);
	}
	EVENT_CALLBACK_DEF(ParticleSystem, HandleRemoveEntity) {
		auto e{ CAST_TO_EVENT(Events::RemoveEntityEvent) };
		ECS::Entity entity{ const_cast<ECS::Entity&>(e->mEntity) };
		if (entity.HasComponent<Component::EmitterSystem>())
			Graphics::ParticleManager::GetInstance().MultiEmitterAction(entity.GetComponent<Component::EmitterSystem>().emitters, -1);
	}
}