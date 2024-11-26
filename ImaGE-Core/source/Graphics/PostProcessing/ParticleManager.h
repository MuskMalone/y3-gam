#pragma once
#include "Core/Entity.h"
#include "glad/glad.h"
#include <queue>

namespace Graphics{
#pragma warning(push)
#pragma warning(disable : 4324)
    namespace GLSLStructs {

        //1-1 map of the structs in Particle/Common.glsl
        //its structured to be packed as 16 bytes with minimal intra object padding
        struct alignas(16) Emitter {
            glm::vec4 vertices[8];    // 8 vec4s, each 16 bytes
            glm::vec4 col;            // 16 bytes
            glm::vec3 pos;            // 12 bytes
            float padding1;           // Align to 16 bytes
            glm::vec3 vel;            // 12 bytes
            float padding2;           // Align to 16 bytes
            glm::vec3 size;           // 12 bytes
            float padding3;           // Align to 16 bytes
            glm::vec3 rot;            // 12 bytes
            float padding4;           // Align to 16 bytes
            glm::vec3 angvel;         // 12 bytes
            float padding5;           // Align to 16 bytes

            float lifetime;           // 4 bytes
            float speed;              // 4 bytes
            float time;               // 4 bytes
            float frequency;          // 4 bytes

            int type;                 // 4 bytes
            int vCount;               // 4 bytes
            int preset;               // 4 bytes
            int particlesPerFrame;    // 4 bytes

            int alive;                // GLSL `bool` is 4 bytes; use `int` for compatibility
            float padding6;           // Align struct size to a multiple of 16 bytes
        };

        struct alignas(16) Particle {
            glm::vec4 startCol;       // 16 bytes
            glm::vec4 col;            // 16 bytes

            glm::vec3 startPos;       // 12 bytes
            float padding1;           // Align to 16 bytes
            glm::vec3 pos;            // 12 bytes
            float padding2;           // Align to 16 bytes

            glm::vec3 startVel;       // 12 bytes
            float padding3;           // Align to 16 bytes
            glm::vec3 vel;            // 12 bytes
            float padding4;           // Align to 16 bytes

            glm::vec3 startSize;      // 12 bytes
            float padding5;           // Align to 16 bytes
            glm::vec3 size;           // 12 bytes
            float padding6;           // Align to 16 bytes

            glm::vec3 rot;            // 12 bytes
            float padding7;           // Align to 16 bytes
            glm::vec3 angvel;         // 12 bytes
            float padding8;           // Align to 16 bytes

            float age;                // 4 bytes
            float lifetime;           // 4 bytes
            int emtIdx;               // 4 bytes
            int alive;                // GLSL `bool` is 4 bytes; use `int` for compatibility
        };
    }
#pragma warning(pop)

    //emitter system component has multiple emitters
    //each emitter is an EmitterInstance
    //not the same as Emitter inside GLSLStructs
    struct EmitterInstance {
        glm::vec4 vertices[4]; // Each vec4 is 16 bytes, total 64 bytes
        glm::vec4 col;         // 16 bytes (vec3 is aligned like vec4)

        glm::vec2 gravity; // 8 bytes
        glm::vec2 size;    // 8 bytes (vec2 is aligned to 8 bytes)
        float rot;    // 4 bytes
        float lifetime; // 4 bytes
        float angvel;  // 4 bytes
        float speed;     // 8 bytes

        float time;       // 4 bytes, but due to the vec3 above, you can expect padding here
        float frequency;  // 4 bytes

        // type of emmission
        //0: smoke
        //1: fire
        //2: burst
        //3: burst with gravity
        //4: gradual emission
        int type;         // 4 bytes
        // 1 for point, 2 for line, 4 for rect
        int vCount{ 1 };       // 4 bytes
        int preset;    // 4 bytes //alpha over lifetime etc
        int particlesPerFrame; // 4 bytes
        int idx{ -1 };
        bool drawEmitterVertices{ false };
    };

	class ParticleManager {
	public:
		void Initialize();
		ParticleManager();

        void EmitterAction(EmitterInstance& emitter, int action);


        void Bind();
        void Unbind();

	private:
		GLuint mEmitterSSbo;
		GLuint mParticleSSbo;
		GLuint mParticleStartSSbo;
		GLuint mParticleCountSSbo;

		//for randomness in glsl
		GLuint mRandomSSbo;
		GLuint mRandomIdxSSbo;

		std::queue<GLuint> mEmitterIdxQueue;
	};
}