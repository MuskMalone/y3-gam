#include <pch.h>
#include "ParticleManager.h"
#include <random>
#include "Graphics/Shader.h"
#include <Events/EventManager.h>
#include "Input/InputManager.h"
#include "Graphics/Renderer.h"
namespace Graphics {
    void ParticleManager::Initialize()
    {
        SUBSCRIBE_CLASS_FUNC(Events::SceneStateChange, &ParticleManager::HandleSystemEvents, this);
        //2 4byte variables to be stored, macros for idx corresponding to each var in Common.glsl
        constexpr int variableCount{ 2 };
        GLuint zero = 0;
        glGenBuffers(1, &mVariableSSbo);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, mVariableSSbo);
        glBufferData(GL_SHADER_STORAGE_BUFFER, variableCount * sizeof(GLuint), NULL, GL_DYNAMIC_DRAW);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(GLuint), &zero);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        //I'm only going to comment one of these, because the other SSBOs are essentially the same
    // Generate the initial buffer
        glGenBuffers(1, &mEmitterSSbo);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, mEmitterSSbo);
        glBufferData(GL_SHADER_STORAGE_BUFFER, MAX_BUFFER * sizeof(GLSLStructs::Emitter), NULL, GL_STATIC_DRAW);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        // Do it again, twice.
        glGenBuffers(1, &mParticleStartSSbo);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, mParticleStartSSbo);
        glBufferData(GL_SHADER_STORAGE_BUFFER, MAX_BUFFER * sizeof(GLSLStructs::Particle), NULL, GL_STATIC_DRAW);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        // Do it again, thrice.
        glGenBuffers(1, &mParticleSSbo);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, mParticleSSbo);
        glBufferData(GL_SHADER_STORAGE_BUFFER, MAX_BUFFER * sizeof(GLSLStructs::Particle), NULL, GL_STATIC_DRAW);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        std::vector<float> randomData(MAX_BUFFER);
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dis(-1.0f, 1.0f);
        for (size_t i = 0; i < MAX_BUFFER; ++i) {
            randomData[i] = dis(gen);
        }
        glGenBuffers(1, &mRandomSSbo);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, mRandomSSbo);
        glBufferData(GL_SHADER_STORAGE_BUFFER, MAX_BUFFER * sizeof(float), NULL, GL_STATIC_DRAW);
        float* ptr = (float*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, MAX_BUFFER * sizeof(float), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
        if (ptr) {
            // Copy the data to the SSBO
            std::memcpy(ptr, randomData.data(), MAX_BUFFER * sizeof(float));
            // Unmap the buffer
            glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
        }
        // Unbind the buffer (optional, for cleanup)
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        // Ensures accesses to the SSBOs "reflect" writes from compute shader
        Bind();
        glMemoryBarrier(GL_ALL_BARRIER_BITS);

        //create
        for (uint64_t i{}; i < MAX_BUFFER; ++i) { mEmitterIdxQueue.push(static_cast<unsigned int>(i)); }

        //for testing purpose
    }

    ParticleManager::ParticleManager()
    {
        Initialize();
    }

    ParticleManager::~ParticleManager()
    {
        glDeleteBuffers(1, &mVariableSSbo);
        glDeleteBuffers(1, &mRandomSSbo);
        glDeleteBuffers(1, &mEmitterSSbo);
        glDeleteBuffers(1, &mParticleSSbo);
        glDeleteBuffers(1, &mParticleStartSSbo);
    }
    void ParticleManager::MultiEmitterAction(std::vector<EmitterInstance>& emitters, int action) {
        for (auto& emitter : emitters) {
            EmitterAction(emitter, action);
        }
    }
    void ParticleManager::EmitterAction(EmitterInstance& emitter, int action) {
        auto emitterShader{ ShaderLibrary::Get("Emitter") };
        emitterShader->Use();

        //gets the first available idx for emitter;
        if (action == 1) {
            emitter.idx = mEmitterIdxQueue.front();
            mEmitterIdxQueue.pop();
        }
        //push the available id back in the queue
        else if (action == -1) {
            mEmitterIdxQueue.push(emitter.idx);
        }
        emitterShader->SetUniform("spawnEmitter", action);
        emitterShader->SetUniform("emtTargetIdx", static_cast<GLint>(emitter.idx));

        if (action >= 0) {
            // Set the emitter struct values in the shader

            // Assuming emitterShader is your shader program instance
            GLint uEmtVerticesLoc = glGetUniformLocation(emitterShader->PgmHdl(), "emitter.vertices");

            // Set the vertices (8 vec4 elements)
            auto const& v{ emitter.vertices };
            GLfloat vertices[32] = { // 8 vec4s, each with 4 components
                v[0][0], v[0][1], v[0][2], v[0][3],
                v[1][0], v[1][1], v[1][2], v[1][3],
                v[2][0], v[2][1], v[2][2], v[2][3],
                v[3][0], v[3][1], v[3][2], v[3][3],
                v[4][0], v[4][1], v[4][2], v[4][3],
                v[5][0], v[5][1], v[5][2], v[5][3],
                v[6][0], v[6][1], v[6][2], v[6][3],
                v[7][0], v[7][1], v[7][2], v[7][3],
            };
            glUniform4fv(uEmtVerticesLoc, 8, vertices);

            // Set the other uniform values individually
            emitterShader->SetUniform("emitter.col", emitter.col);
            emitterShader->SetUniform("emitter.vel", emitter.vel);
            emitterShader->SetUniform("emitter.spreadAngle", emitter.spreadAngle);
            emitterShader->SetUniform("emitter.gravity", emitter.gravity);
            emitterShader->SetUniform("emitter.size", emitter.size);

            emitterShader->SetUniform("emitter.angvel", emitter.angvel);
            emitterShader->SetUniform("emitter.lifetime", emitter.lifetime);
            emitterShader->SetUniform("emitter.speed", emitter.speed);

            emitterShader->SetUniform("emitter.time", emitter.time);
            emitterShader->SetUniform("emitter.frequency", emitter.frequency);

            emitterShader->SetUniform("emitter.type", emitter.type);
            emitterShader->SetUniform("emitter.vCount", emitter.vCount);
            emitterShader->SetUniform("emitter.preset", emitter.preset);
            emitterShader->SetUniform("emitter.particlesPerFrame", emitter.particlesPerFrame);

            emitterShader->SetUniform("emitter.alive", emitter.active);
        }

        glDispatchCompute(1, 1, 1);
        glMemoryBarrier(GL_ALL_BARRIER_BITS);

        //set the flags back
        //mParticleShader->SetUniform("spawnEmitter", 0);
        emitterShader->SetUniform("emtTargetIdx", -1);

        emitterShader->Unuse();
    }

    void ParticleManager::DebugSSBO()
    {
        //glBindBuffer(GL_SHADER_STORAGE_BUFFER, mEmitterSSbo);
        //GLSLStructs::Emitter* vels = (GLSLStructs::Emitter*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, MAX_BUFFER * sizeof(GLSLStructs::Emitter), GL_MAP_READ_BIT);
        //glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

        //glBindBuffer(GL_SHADER_STORAGE_BUFFER, mParticleStartSSbo);
        //GLSLStructs::Particle* vels = (GLSLStructs::Particle*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, MAX_BUFFER * sizeof(GLSLStructs::Particle), GL_MAP_READ_BIT);
        //glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

        //glBindBuffer(GL_SHADER_STORAGE_BUFFER, mRandomSSbo);
        //float* vels = (float*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, MAX_BUFFER * sizeof(float), GL_MAP_READ_BIT);
        //glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

        //glBindBuffer(GL_SHADER_STORAGE_BUFFER, mVariableSSbo);
        //GLuint* idx = (GLuint*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, sizeof(GLuint), GL_MAP_READ_BIT);
        //glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    }


    void ParticleManager::Debug()
    {
        if (Input::InputManager::GetInstance().IsKeyHeld(KEY_CODE::KEY_LEFT_CONTROL) &&
            Input::InputManager::GetInstance().IsKeyTriggered(KEY_CODE::KEY_D))
            mDebug = !mDebug;
        if (!mDebug) return;

        for (auto const& emittervecs : mDebugEmitters) {
            for (auto const& emitter : emittervecs) {
                if (emitter.vCount < 8) {
                    for (int i = 0; i < emitter.vCount; ++i) {
                        // Get the position from the vertex (using the first 3 components of vec4)
                        glm::vec3 pos(emitter.vertices[i].x, emitter.vertices[i].y, emitter.vertices[i].z);
                        // Draw a sphere at the vertex with a radius of 0.1 and the emitter's color.
                        Graphics::Renderer::DrawWireSphere(pos, 0.1f, emitter.col);

                        // Determine the next vertex index (wrap-around to form a closed shape)
                        int nextIndex = (i + 1) % emitter.vCount;
                        glm::vec3 nextPos(emitter.vertices[nextIndex].x, emitter.vertices[nextIndex].y, emitter.vertices[nextIndex].z);
                        // Draw a line connecting this vertex to the next vertex.
                        Graphics::Renderer::DrawLine(pos, nextPos, emitter.col);
                    }
                }
                else {
                    // For an irregular cube (vCount == 8) assume vertices are ordered as:
                    //  0: near top left, 1: near top right, 2: far top right, 3: far top left,
                    //  4: near bottom left, 5: near bottom right, 6: far bottom right, 7: far bottom left.
                    glm::vec3 v[8];
                    for (int i = 0; i < 8; ++i) {
                        v[i] = glm::vec3(emitter.vertices[i].x, emitter.vertices[i].y, emitter.vertices[i].z);
                        // Draw each vertex.
                        Graphics::Renderer::DrawWireSphere(v[i], 0.1f, emitter.col);
                    }

                    // Draw top face edges.
                    Graphics::Renderer::DrawLine(v[0], v[1], emitter.col); // near top left -> near top right
                    Graphics::Renderer::DrawLine(v[1], v[2], emitter.col); // near top right -> far top right
                    Graphics::Renderer::DrawLine(v[2], v[3], emitter.col); // far top right -> far top left
                    Graphics::Renderer::DrawLine(v[3], v[0], emitter.col); // far top left -> near top left

                    // Draw bottom face edges.
                    Graphics::Renderer::DrawLine(v[4], v[5], emitter.col); // near bottom left -> near bottom right
                    Graphics::Renderer::DrawLine(v[5], v[6], emitter.col); // near bottom right -> far bottom right
                    Graphics::Renderer::DrawLine(v[6], v[7], emitter.col); // far bottom right -> far bottom left
                    Graphics::Renderer::DrawLine(v[7], v[4], emitter.col); // far bottom left -> near bottom left

                    // Draw vertical edges connecting top and bottom.
                    Graphics::Renderer::DrawLine(v[0], v[4], emitter.col); // near top left -> near bottom left
                    Graphics::Renderer::DrawLine(v[1], v[5], emitter.col); // near top right -> near bottom right
                    Graphics::Renderer::DrawLine(v[2], v[6], emitter.col); // far top right -> far bottom right
                    Graphics::Renderer::DrawLine(v[3], v[7], emitter.col); // far top left -> far bottom left
                }


            }
        }

        mDebugEmitters.clear();
    }

    void ParticleManager::Bind()
    {
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 11, mVariableSSbo);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 12, mRandomSSbo);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 13, mEmitterSSbo);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 14, mParticleSSbo);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 15, mParticleStartSSbo);
    }
    void ParticleManager::Unbind()
    {
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 11, 0);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 12, 0);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 13, 0);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 14, 0);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 15, 0);
    }
    void ParticleManager::ClearParticleBuffer()
    {
        // Bind the buffer
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, mParticleSSbo);

        // Map the entire buffer for writing.
        // We use GL_MAP_WRITE_BIT to write and GL_MAP_INVALIDATE_BUFFER_BIT to tell OpenGL
        // that we don't care about the previous contents.
        GLSLStructs::Particle* data = (GLSLStructs::Particle*)
            glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, MAX_BUFFER * sizeof(GLSLStructs::Particle),
                GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

        if (data)
        {
            // Clear the buffer by setting all bytes to zero.
            memset(data, 0, MAX_BUFFER * sizeof(GLSLStructs::Particle));

            // Unmap the buffer so that the changes take effect.
            glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
        }

        //do it again
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, mParticleStartSSbo);
        data = (GLSLStructs::Particle*)
            glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, MAX_BUFFER * sizeof(GLSLStructs::Particle),
                GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
        if (data)
        {
            memset(data, 0, MAX_BUFFER * sizeof(GLSLStructs::Particle));
            glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
        }

        //and again
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, mEmitterSSbo);
        GLSLStructs::Emitter* data2 = (GLSLStructs::Emitter*)
            glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, MAX_BUFFER * sizeof(GLSLStructs::Emitter),
                GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
        if (data2)
        {
            memset(data2, 0, MAX_BUFFER * sizeof(GLSLStructs::Emitter));
            glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
        }
    }
    EVENT_CALLBACK_DEF(ParticleManager, HandleSystemEvents) {
        auto const& state{ CAST_TO_EVENT(Events::SceneStateChange)->mNewState };

        if (state == Events::SceneStateChange::NewSceneState::STOPPED || state == Events::SceneStateChange::NewSceneState::CHANGED) {
            ClearParticleBuffer();
        }
    }
}