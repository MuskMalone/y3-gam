#pragma once
#include "Core/Entity.h"
#include "glad/glad.h"
#include <queue>
#include "Singleton/ThreadSafeSingleton.h"
#define MAX_BUFFER 1000000
#define WORK_GROUP 1000 //max buffer should be divisible by work group
namespace Graphics{
#pragma warning(push)
#pragma warning(disable : 4324)
    namespace GLSLStructs {

        //1-1 map of the structs in Particle/Common.glsl
        //its structured to be packed as 16 bytes with minimal intra object padding
// Ensure 16-byte alignment for compatibility with std430 layout
        struct alignas(16) Emitter {
            glm::vec4 vertices[8]; // 8 vec4s, 64 bytes
            glm::vec4 col;                    // Color, 16 bytes

            glm::vec3 vel;                    // Velocity, 12 bytes
            float _padding1;                  // Padding for 16-byte alignment

            glm::vec3 rot;                    // Rotation, 12 bytes
            float _padding2;                  // Padding for 16-byte alignment

            glm::vec2 size;                   // Size, 8 bytes
            float angvel;                     // Angular velocity, 4 bytes
            float lifetime;                   // Lifetime, 4 bytes

            float speed;                      // Speed, 4 bytes
            float time;                       // Total emitter time, 4 bytes
            float frequency;                  // Emission frequency, 4 bytes

            int type;                         // Type, 4 bytes
            int vCount;                       // Vertex count, 4 bytes
            int preset;                       // Preset, 4 bytes
            int particlesPerFrame;            // Particles emitted per frame, 4 bytes

            bool alive;                       // Active flag, 1 byte
            char _padding3[3];                // Padding for 16-byte alignment
        };

        // Ensure 16-byte alignment for compatibility with std430 layout
        struct alignas(16) Particle {
            glm::vec4 col;                    // Current color, 16 bytes

            glm::vec3 pos;                    // Current position, 12 bytes
            float _padding1;                  // Padding for 16-byte alignment

            glm::vec3 vel;                    // Current velocity, 12 bytes
            float _padding2;                  // Padding for 16-byte alignment

            glm::vec3 rot;                    // Rotation, 12 bytes
            float angvel;                     // Angular velocity, 4 bytes

            glm::vec2 size;                   // Size, 8 bytes
            float age;                        // Age, 4 bytes
            float lifetime;                   // Lifetime, 4 bytes

            int emtIdx;                       // Emitter index, 4 bytes
            bool alive;                       // Active flag, 1 byte
            char _padding3[3];                // Padding for 16-byte alignment
        };
    }
#pragma warning(pop)

    //emitter system component has multiple emitters
    //each emitter is an EmitterInstance
    //not the same as Emitter inside GLSLStructs
    struct EmitterInstance {
        glm::vec4 vertices[8];  // 4 vec4s, each vec4 is 16 bytes, total 64 bytes
        //color
        glm::vec4 col;          // 16 bytes
        glm::vec3 vel;          // vec3
        glm::vec3 rot;
        glm::vec2 size;         // vec2

        //since particles are billboard, means that angle velocity is 2d
        float angvel;
        //lifetime of each particle
        float lifetime;         // 4 bytes
        float speed;            // 4 bytes

        // total duration the Emitter has existed for
        float time;             // 4 bytes
        float frequency;        // 4 bytes

        int type;               // 4 bytes
        int vCount;             // 4 bytes
        int preset;             // 4 bytes
        int particlesPerFrame;  // 4 bytes

        int idx{ -1 };
        bool drawEmitterVertices{ false };
    };

	class ParticleManager : public ThreadSafeSingleton<ParticleManager> {
	public:
		void Initialize();
		ParticleManager();
        ~ParticleManager();
        void EmitterAction(EmitterInstance& emitter, int action);
        void DebugSSBO();
        void Bind();
        void Unbind();

	private:
		GLuint mEmitterSSbo;
		GLuint mParticleSSbo;
		GLuint mParticleStartSSbo;

		//for randomness in glsl
		GLuint mRandomSSbo;
		GLuint mVariableSSbo;

		std::queue<GLuint> mEmitterIdxQueue;
	};
}