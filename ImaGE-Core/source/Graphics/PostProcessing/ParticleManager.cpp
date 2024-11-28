#include <pch.h>
#include "ParticleManager.h"
#include <random>
#include "Graphics/Shader.h"
#define MAX_BUFFER 1000000
#define WORK_GROUP 1000 //max buffer should be divisible by work group

namespace Graphics{
    void ParticleManager::Initialize()
    {
        GLuint zero = 0;
        glGenBuffers(1, &mRandomIdxSSbo);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, mRandomIdxSSbo);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GLuint), NULL, GL_DYNAMIC_DRAW);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(GLuint), &zero);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        glGenBuffers(1, &mParticleCountSSbo);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, mParticleCountSSbo);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GLuint), NULL, GL_DYNAMIC_DRAW);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(GLuint), &zero);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        //I'm only going to comment one of these, because the other SSBOs are essentially the same
    // Generate the initial buffer
        glGenBuffers(1, &mEmitterSSbo);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, mEmitterSSbo);
        glBufferData(GL_SHADER_STORAGE_BUFFER, MAX_BUFFER * sizeof(GLSLStructs::Emitter), NULL, GL_STATIC_DRAW);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        // Do it again, twice.
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
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        //mEmitterShader = std::make_shared<Shader>("../assets/shaders/Emitter.glsl");
        //mEmitterStepShader = std::make_shared<Shader>("../assets/shaders/EmitterStep.glsl");
        //mParticleShader = std::make_shared<Shader>("../assets/shaders/Particle.glsl");
        //mParticleRenderShader = std::make_shared<Shader>("../assets/shaders/Particle.geom", "../assets/shaders/Particle.vert", "../assets/shaders/Particle.frag");

        for (uint64_t i{}; i < MAX_BUFFER; ++i) { mEmitterIdxQueue.push(static_cast<unsigned int>(i)); }
    }

    ParticleManager::ParticleManager()
    {
    }

    inline void ParticleManager::EmitterAction(EmitterInstance& emitter, int action) {
        //mEmitterShader->Use();
        auto emitterShader{ ShaderLibrary::Get("Emitter") };
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
            //sets the vertices
            GLint uEmtverticesLoc = glGetUniformLocation(emitterShader->PgmHdl(), "uEmtvertices");

            //sets the emitter shape
            auto const& v{ emitter.vertices };
            GLfloat vertices[16] = {
                // vec4 1
                v[0][0], v[0][1], v[0][2], v[0][3],
                // vec4 2
                v[1][0], v[1][1], v[1][2], v[1][3],
                // vec4 3
                v[2][0], v[2][1], v[2][2], v[2][3],
                // vec4 4
                v[3][0], v[3][1], v[3][2], v[3][3],
            };
            glUniform4fv(uEmtverticesLoc, 4, vertices);

            emitterShader->SetUniform("uEmtcol", emitter.col);

            emitterShader->SetUniform("uEmtgravity", emitter.gravity);
            emitterShader->SetUniform("uEmtsize", emitter.size);
            emitterShader->SetUniform("uEmtrot", emitter.rot);
            emitterShader->SetUniform("uEmtlifetime", emitter.lifetime);
            emitterShader->SetUniform("uEmtangvel", emitter.angvel);
            emitterShader->SetUniform("uEmtspeed", emitter.speed);

            emitterShader->SetUniform("uEmtfrequency", emitter.frequency);
            emitterShader->SetUniform("uEmttype", emitter.type);
            emitterShader->SetUniform("uEmtvCount", emitter.vCount);
            emitterShader->SetUniform("uEmtpreset", emitter.preset);
            emitterShader->SetUniform("uEmtparticlesPerFrame", emitter.particlesPerFrame);


        }

        glDispatchCompute(1, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        //set the flags back
        //mParticleShader->SetUniform("spawnEmitter", 0);
        emitterShader->SetUniform("emtTargetIdx", -1);

        emitterShader->Unuse();
    }

    void ParticleManager::Bind()
    {
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 11, mRandomIdxSSbo);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 12, mRandomSSbo);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 13, mEmitterSSbo);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 14, mParticleSSbo);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 15, mParticleCountSSbo);
    }
    void ParticleManager::Unbind()
    {
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 11, 0);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 12, 0);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 13, 0);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 14, 0);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 15, 0);
    }
}
