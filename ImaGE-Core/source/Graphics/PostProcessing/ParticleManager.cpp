#include <pch.h>
#include "ParticleManager.h"
#include <random>
#include "Graphics/Shader.h"


namespace Graphics{
    void ParticleManager::Initialize()
    {
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
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

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

        }

        glDispatchCompute(1, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

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
}
