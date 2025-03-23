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
            auto const& rot{ e.GetComponent<Component::Transform>().worldRot };
            auto const& sca{ e.GetComponent<Component::Transform>().worldScale };
			auto const& tag{ e.GetComponent<Component::Tag>().tag };
			//update the transforms
			for (int i{}; i < vecProxy.size(); ++i) {
				auto& emitproxy{ vecProxy[i] };
				if (e.IsActive()) {
					emitproxy.active = true;
				}
				else {
					emitproxy.active = false;
				}
                if (emitproxy.vCount < 8) {
                    for (int j = 0; j < emitproxy.vCount; ++j) {
                        // Get the original local vertex.
                        glm::vec3 localVertex = glm::vec3(emitproxy.vertices[j]);
                        // Scale it (component-wise multiplication).
                        glm::vec3 scaled = localVertex * sca;
                        // Rotate it.
                        glm::vec3 rotated = rot * scaled;
                        // Translate it.
                        glm::vec3 worldVertex = pos + rotated;
                        // Write back to the vertex (preserving w if needed).
                        emitproxy.vertices[j][0] = worldVertex.x;
                        emitproxy.vertices[j][1] = worldVertex.y;
                        emitproxy.vertices[j][2] = worldVertex.z;
                    }
                }
                else {
                    // Reorder the 8 unordered vertices into the canonical cube order.
                    // For simplicity we assume the following:
                    //   - The top face is determined by the 4 vertices with y >= center.y
                    //   - The bottom face is the remaining 4 vertices.
                    //   - Within each group, sort by z (near to far) then by x (left to right).
                    // This gives:
                    //   Top Face: 0: near top left, 1: near top right, 2: far top right, 3: far top left
                    //   Bottom Face: 4: near bottom left, 5: near bottom right, 6: far bottom right, 7: far bottom left

                    // Compute center
                    glm::vec3 center(0.0f);
                    for (int i = 0; i < 8; ++i) {
                        center += glm::vec3(emitproxy.vertices[i]);
                    }
                    center /= 8.0f;

                    std::vector<glm::vec4> topVerts, bottomVerts;
                    for (int i = 0; i < 8; ++i) {
                        glm::vec3 v = glm::vec3(emitproxy.vertices[i]);
                        if (v.y >= center.y)
                            topVerts.push_back(emitproxy.vertices[i]);
                        else
                            bottomVerts.push_back(emitproxy.vertices[i]);
                    }

                    // If grouping fails, fallback to simply applying offset.
                    if (topVerts.size() != 4 || bottomVerts.size() != 4) {
                        for (int i = 0; i < 8; ++i) {
                            emitproxy.vertices[i][0] += pos.x;
                            emitproxy.vertices[i][1] += pos.y;
                            emitproxy.vertices[i][2] += pos.z;
                        }
                    }
                    else {
                        auto sortByZ = [](const glm::vec4& a, const glm::vec4& b) {
                            return glm::vec3(a).z < glm::vec3(b).z;
                            };
                        std::sort(topVerts.begin(), topVerts.end(), sortByZ);
                        std::sort(bottomVerts.begin(), bottomVerts.end(), sortByZ);

                        // Split each group into near (first two) and far (last two)
                        std::vector<glm::vec4> topNear = { topVerts[0], topVerts[1] };
                        std::vector<glm::vec4> topFar = { topVerts[2], topVerts[3] };
                        std::vector<glm::vec4> bottomNear = { bottomVerts[0], bottomVerts[1] };
                        std::vector<glm::vec4> bottomFar = { bottomVerts[2], bottomVerts[3] };

                        auto sortByX = [](const glm::vec4& a, const glm::vec4& b) {
                            return glm::vec3(a).x < glm::vec3(b).x;
                            };
                        std::sort(topNear.begin(), topNear.end(), sortByX);
                        std::sort(topFar.begin(), topFar.end(), sortByX);
                        std::sort(bottomNear.begin(), bottomNear.end(), sortByX);
                        std::sort(bottomFar.begin(), bottomFar.end(), sortByX);

                        // Order the top face: near left, near right, far right, far left.
                        std::vector<glm::vec4> orderedTop;
                        orderedTop.push_back(topNear[0]);   // near top left
                        orderedTop.push_back(topNear[1]);   // near top right
                        orderedTop.push_back(topFar[1]);    // far top right
                        orderedTop.push_back(topFar[0]);    // far top left

                        // Order the bottom face similarly.
                        std::vector<glm::vec4> orderedBottom;
                        orderedBottom.push_back(bottomNear[0]); // near bottom left
                        orderedBottom.push_back(bottomNear[1]); // near bottom right
                        orderedBottom.push_back(bottomFar[1]);  // far bottom right
                        orderedBottom.push_back(bottomFar[0]);  // far bottom left

                        // Combine top and bottom.
                        std::vector<glm::vec4> orderedVertices;
                        orderedVertices.insert(orderedVertices.end(), orderedTop.begin(), orderedTop.end());
                        orderedVertices.insert(orderedVertices.end(), orderedBottom.begin(), orderedBottom.end());

                        // Write the ordered vertices back, applying the global offset.
                        for (int i = 0; i < 8; ++i) {
                            glm::vec3 localVertex = glm::vec3(orderedVertices[i]);
                            glm::vec3 worldVertex = pos + (rot * (localVertex * sca));
                            emitproxy.vertices[i][0] = worldVertex.x;
                            emitproxy.vertices[i][1] = worldVertex.y;
                            emitproxy.vertices[i][2] = worldVertex.z;
                            emitproxy.vertices[i][3] = orderedVertices[i].w; // preserve w
                        }
                    }
                }
                Graphics::ParticleManager::GetInstance().EmitterAction(emitproxy, 0);
				//e.GetComponent<Component::EmitterSystem>().emitters[i].active = false;
			}
			if (Graphics::ParticleManager::GetInstance().mDebug)
				Graphics::ParticleManager::GetInstance().mDebugEmitters.push_back(std::move(vecProxy));
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
		if (entity.HasComponent<Component::EmitterSystem>())
			Graphics::ParticleManager::GetInstance().MultiEmitterAction(entity.GetComponent<Component::EmitterSystem>().emitters, -1);
	}
	EVENT_CALLBACK_DEF(ParticleSystem, HandleRemoveEntity) {
		auto e{ CAST_TO_EVENT(Events::RemoveEntityEvent) };
		ECS::Entity entity{ const_cast<ECS::Entity&>(e->mEntity) };
		if (entity.HasComponent<Component::EmitterSystem>())
			Graphics::ParticleManager::GetInstance().MultiEmitterAction(entity.GetComponent<Component::EmitterSystem>().emitters, -1);
	}
}