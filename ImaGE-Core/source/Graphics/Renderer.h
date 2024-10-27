/*!*********************************************************************
\file   Renderer.h
\author k.choa\@digipen.edu
\date   22/09/2024
\brief  The Renderer class is responsible for setting up and managing the rendering pipeline, including textures,
		shaders, vertex buffers, and framebuffers. It supports batch rendering for quads, triangles, and meshes,
		optimizing performance by minimizing draw calls. The class also supports instanced rendering and handles
		multiple render passes (e.g., geometry pass, picking pass).

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#pragma once
#include <vector>
#include <glm/glm.hpp>
#pragma region RenderPasses
#include <Graphics/RenderPass/GeomPass.h>
#include <Graphics/RenderPass/ShadowPass.h>
#pragma endregion
#include "Shader.h"
#include "Texture.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "Color.h"

namespace Graphics {
	class Material; class Mesh;

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

	struct InstanceData {
		glm::mat4 modelMatrix;
		int materialIdx;
		int entityID = -1;
		//glm::vec4 color;
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

		//static const uint32_t cMaxQuads{ 20000 };
		//static const uint32_t cMaxVertices{ cMaxQuads * 4 };
		//static const uint32_t cMaxIndices{ cMaxQuads * 6 };

		std::shared_ptr<VertexArray> triVertexArray;
		std::shared_ptr<VertexBuffer> triVertexBuffer;

		std::shared_ptr<VertexArray> quadVertexArray;
		std::shared_ptr<VertexBuffer> quadVertexBuffer;
		std::shared_ptr<Shader> texShader;
		IGE::Assets::GUID defaultTex;
		IGE::Assets::GUID whiteTex;

		std::shared_ptr<Shader> lineShader;
		std::shared_ptr<Shader> instancedShader;

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

		std::unordered_map<IGE::Assets::GUID, std::vector<InstanceData>> instanceBufferDataMap;
		std::unordered_map<IGE::Assets::GUID, std::shared_ptr<VertexBuffer>> instanceBuffers;

		Statistics stats;

		//TEMP FOR NOW
		std::vector<std::shared_ptr<Material>> materialVector;
		std::vector<IGE::Assets::GUID> albedoMaps;
		std::vector<IGE::Assets::GUID> normalMaps;
	};

	class Renderer {
	public:

		static void Init();
		static void Shutdown();

		// Quads
		static void DrawQuad(glm::vec3 const& pos, glm::vec2 const& scale, glm::vec4 const& clr, float rot = 0.f);

		static void SubmitMesh(std::shared_ptr<Mesh> mesh, glm::vec3 const& pos, glm::vec3 const& rot, glm::vec3 const& scale, glm::vec4 const& clr = Color::COLOR_WHITE);
		static void SubmitTriangle(glm::vec3 const& v1, glm::vec3 const& v2, glm::vec3 const& v3, glm::vec4 const& clr = Color::COLOR_WHITE);

		//Instancing
		static void SubmitInstance(IGE::Assets::GUID meshSource, glm::mat4 const& worldMtx, glm::vec4 const& clr, int entityID = -1, uint32_t matID = 0);
		static void RenderInstances();

		// Batching
		static void BeginBatch();
		static void FlushBatch();
		static void FlushBatch(std::shared_ptr<RenderPass> const& renderPass);

		static void RenderSceneBegin(glm::mat4 const& viewProjMtx);
		static void RenderSceneEnd();

		static std::shared_ptr<Material> GetMaterial(uint32_t idx); //temp
		static std::vector<IGE::Assets::GUID> const& GetAlbedoMaps();//temp
		static std::vector<IGE::Assets::GUID> const& GetNormalMaps();

		static unsigned int GetMaxTextureUnits();
		static std::shared_ptr<Graphics::Framebuffer> GetFinalFramebuffer();
		static void SetFinalFramebuffer(std::shared_ptr<Graphics::Framebuffer> const& framebuffer);
		static IGE::Assets::GUID GetDefaultTexture();
		static IGE::Assets::GUID GetWhiteTexture();
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

		static std::shared_ptr<VertexBuffer> GetInstanceBuffer(IGE::Assets::GUID const& meshSrc);
		//static void SetQuadBufferData(const glm::vec3& pos, const glm::vec2& scale,
		//	const glm::vec4& clr, const glm::vec2& texCoord, float texIdx, int entity);
		static void NextBatch();

		static void InitPickPass();
		static void InitGeomPass();
		static void InitShadowMapPass();

		// Stats
		static Statistics GetStats();
		static void ResetStats();

	private:
		static RendererData mData;
		static std::shared_ptr<Framebuffer> mFinalFramebuffer;

	public: // TEMP
		static std::vector<std::shared_ptr<RenderPass>> mRenderPasses;
	};
}