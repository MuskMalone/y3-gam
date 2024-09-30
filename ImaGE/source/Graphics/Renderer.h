#pragma once
#include "Shader.h"
#include "Texture.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "ElementBuffer.h"
#include "Mesh.h"
#include "RenderPass.h"

namespace Graphics {
	struct Statistics {
		uint32_t drawCalls{};
		uint32_t quadCount{};

		uint32_t GetTotalVtxCount() { return quadCount * 4; }
		uint32_t GetTotalIdxCount() { return quadCount * 6; }
	};

	struct QuadVtx {
		glm::vec3 pos;
		glm::vec3 normal;
		glm::vec2 texCoord;
		float texIdx; // float as it is passed to shader
		glm::vec3 tangent;
		glm::vec3 bitangent;
		glm::vec4 clr;
	};

	struct TriVtx {
		glm::vec3 pos;
		glm::vec4 clr;
	};

	struct RendererData {
		uint32_t maxTexUnits{};

		//static const uint32_t cMaxMeshes = 1;
		//static const uint32_t cMaxVertices = cMaxMeshes * 100; // estimate based on average mesh complexity
		//static const uint32_t cMaxIndices = cMaxMeshes * 360;

		static const uint32_t cMaxVertices = 500000;
		static const uint32_t cMaxIndices = cMaxVertices * 3;

		std::shared_ptr<VertexArray> meshVertexArray;
		std::shared_ptr<VertexBuffer> meshVertexBuffer;

		uint32_t meshIdxCount{}; //per batch
		uint32_t meshVtxCount{}; //per batch

		std::vector<Vertex> meshBuffer; // Store all vertex data here
		std::vector<uint32_t> meshIdxBuffer; // Store all index data here

		//uint32_t meshBufferIdx = 0;

		static const uint32_t cMaxQuads{ 20000 };
		//static const uint32_t cMaxVertices{ cMaxQuads * 4 };
		//static const uint32_t cMaxIndices{ cMaxQuads * 6 };

		static const uint32_t cMaxCubes{ 5000 }; // Max number of cubes
		//static const uint32_t cMaxVertices{ cMaxCubes * 24 }; // 6 faces * 4 vertices per cube
		//static const uint32_t cMaxIndices{ cMaxCubes * 360 };  // 6 faces * 6 indices per face

		std::shared_ptr<VertexArray> triVertexArray;
		std::shared_ptr<VertexBuffer> triVertexBuffer;

		std::shared_ptr<VertexArray> quadVertexArray;
		std::shared_ptr<VertexBuffer> quadVertexBuffer;
		std::shared_ptr<Shader> texShader;
		std::shared_ptr<Texture> whiteTex;

		std::shared_ptr<Shader> lineShader;

		uint32_t quadIdxCount{};
		uint32_t triVtxCount{};
		std::vector<QuadVtx> quadBuffer; // Dynamic buffer to hold vertex data for batching
		std::vector<TriVtx> triBuffer; // Dynamic buffer to hold vertex data for batching
		//QuadVtx* quadBufferPtr{ nullptr }; // Pointer to the current position in the buffer

		uint32_t quadBufferIndex = 0;     // Index into the quadBuffer instead of a pointer
		uint32_t triBufferIndex = 0;     // Index into the quadBuffer instead of a pointer

		std::array<glm::vec4, 4> quadVtxPos{};

		std::vector<std::shared_ptr<Texture>> texUnits; // Array of Texture pointers
		uint32_t texUnitIdx{ 1 }; // 0 = white tex

		Statistics stats;
	};

	class Renderer {
	public:

		static void Init();
		static void Shutdown();

		// Quads
		static void DrawQuad(glm::vec3 const& pos, glm::vec2 const& scale, glm::vec4 const& clr, float rot = 0.f);

		static void SubmitMesh(std::shared_ptr<Mesh> mesh, glm::vec3 const& pos, glm::vec3 const& scale, glm::vec4 const& clr = { 1.f,1.f,1.f,1.f }, float rot = 0.f);
		static void SubmitTriangle(glm::vec3 const& v1, glm::vec3 const& v2, glm::vec3 const& v3, glm::vec4 const& clr = { 1.f,1.f,1.f,1.f });

		// Batching
		static void BeginBatch();
		static void FlushBatch();
		static void FlushBatch(std::shared_ptr<RenderPass> const& renderPass);

		static void RenderSceneBegin(glm::mat4 const& viewProjMtx);
		static void RenderSceneEnd();

		static unsigned int GetMaxTextureUnits();
		static std::shared_ptr<Graphics::Framebuffer> GetFinalFramebuffer();
		static void SetFinalFramebuffer(std::shared_ptr<Graphics::Framebuffer> const& framebuffer);
	private:
		static void SetQuadBufferData(glm::vec3 const& pos, glm::vec2 const& scale,
			glm::vec3 const& norm, glm::vec2 const& texCoord,
			float texIdx, glm::vec3 const& tangent, glm::vec3 const& bitangent,
			glm::vec4 const& clr);

		static void SetTriangleBufferData(glm::vec3 const& pos, glm::vec4 const& clr);

		static void SetMeshBufferData(glm::vec3 const& pos, glm::vec3 const& norm,
			glm::vec2 const& texCoord, float texIdx,
			glm::vec3 const& tangent, glm::vec3 const& bitangent,
			glm::vec4 const& clr);

		//static void SetQuadBufferData(const glm::vec3& pos, const glm::vec2& scale,
		//	const glm::vec4& clr, const glm::vec2& texCoord, float texIdx, int entity);
		static void NextBatch();

		// Stats
		static Statistics GetStats();
		static void ResetStats();

	private:
		static RendererData mData;
		static std::shared_ptr<Framebuffer> mFinalFramebuffer;
	public: // temp
		static std::shared_ptr<RenderPass> mGeomPass;
	};
}