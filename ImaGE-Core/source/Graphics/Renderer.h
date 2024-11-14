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
#include "Shader.h"
#include "Texture.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "Color.h"
#include "MaterialTable.h"
#include <typeindex>
#include <Graphics/RenderPass/RenderPass.h>
#include "Core/Components/Camera.h"
#include "MeshSubmeshKeyHash.h"


namespace Component{
	struct Camera;
}

namespace Graphics {
	using MeshSubmeshKey = std::pair<IGE::Assets::GUID, uint32_t>;
	class RenderPass; // Forward declaration
	
	class Mesh;

	struct Statistics {
		uint32_t drawCalls{};
		uint32_t quadCount{};
		uint32_t lineCount{};

		uint32_t GetTotalVtxCount() { return quadCount * 4; }
		uint32_t GetTotalIdxCount() { return quadCount * 6; }
	};

	struct FullscreenQuad {
		struct ScreenVtx {
			glm::vec2 pos;
			glm::vec2 texCoord;
		};

		std::shared_ptr<VertexArray> screenVertexArray;
		std::shared_ptr<VertexBuffer> screenVertexBuffer;
		std::array<ScreenVtx, 6> screenVertices;
	};

	struct QuadVtx {
		glm::vec3 pos;
		glm::vec4 clr;
		glm::vec2 texCoord;
		float texIdx; // float as it is passed to shader
		//int entity{};
	};

	struct LineVtx {
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

		//------------------Mesh Batching-----------------------------------//
		static const uint32_t cMaxVertices = 500000;
		static const uint32_t cMaxIndices = cMaxVertices * 3;

		std::shared_ptr<VertexArray> meshVertexArray;
		std::shared_ptr<VertexBuffer> meshVertexBuffer;

		uint32_t meshIdxCount{}; //per batch
		uint32_t meshVtxCount{}; //per batch

		std::vector<Vertex> meshBuffer; // Store all vertex data here
		std::vector<uint32_t> meshIdxBuffer; // Store all index data here
		//------------------------------------------------------------------//

		//------------------2D Quad Batching--------------------------------//
		
		static const uint32_t cMaxQuads{ 20000 };
		static const uint32_t cMaxVertices2D{ cMaxQuads * 4 };
		static const uint32_t cMaxIndices2D{ cMaxQuads * 6 };

		std::shared_ptr<VertexArray> quadVertexArray;
		std::shared_ptr<VertexBuffer> quadVertexBuffer;

		uint32_t quadIdxCount{};
		std::vector<QuadVtx> quadBuffer; // Dynamic buffer to hold vertex data for batching
		uint32_t quadBufferIndex = 0;     // Index into the quadBuffer instead of a pointer

		std::array<glm::vec4, 4> quadVtxPos{};

		/* 2D sprite related */
		std::vector<Texture> texUnits; // Array of Texture pointers
		uint32_t texUnitIdx{ 1 }; // 0 = white tex
		//------------------------------------------------------------------//

		//-----------------Line Batching--------------------------------//
		std::shared_ptr<VertexArray> lineVertexArray;
		std::shared_ptr<VertexBuffer> lineVertexBuffer;


		uint32_t lineVtxCount{};

		std::vector<LineVtx> lineBuffer; // Dynamic buffer to hold vertex data for batching
		uint32_t lineBufferIndex = 0;     // Index into the quadBuffer instead of a pointer
		//-------------------------------------------------------------------//

		//-----------------Triangle Batching--------------------------------//
		std::shared_ptr<VertexArray> triVertexArray;
		std::shared_ptr<VertexBuffer> triVertexBuffer;

		uint32_t triVtxCount{};

		std::vector<LineVtx> triBuffer; // Dynamic buffer to hold vertex data for batching
		uint32_t triBufferIndex = 0;     // Index into the quadBuffer instead of a pointer
		//-------------------------------------------------------------------//

		IGE::Assets::GUID defaultTex;
		IGE::Assets::GUID whiteTex;

		//------------------------Instancing related-------------------------//
		std::unordered_map<IGE::Assets::GUID, std::vector<InstanceData>> instanceBufferDataMap;
		std::unordered_map<IGE::Assets::GUID, std::shared_ptr<VertexBuffer>> instanceBuffers;

		std::unordered_map<MeshSubmeshKey, std::vector<InstanceData>> instanceSubmeshBufferDataMap;
		std::unordered_map<MeshSubmeshKey, std::shared_ptr<VertexBuffer>> instanceSubmeshBuffers;
		//-------------------------------------------------------------------//

		IGE::Assets::GUID debugMeshSources[3];
		IGE::Assets::GUID quadMeshSource;

		FullscreenQuad screen;

		Statistics stats;
	};

	class Renderer {
	public:

		static void Init();
		static void Shutdown();
		static void Clear();

		// Batching
		static void DrawLine(glm::vec3 const& p0, glm::vec3 const& p1, glm::vec4 const& clr);
		static void DrawRect(glm::vec3 const& pos, glm::vec2 const& scale, glm::quat const& rot, glm::vec4 const& clr);
		static void DrawQuad(glm::vec3 const& pos, glm::vec2 const& scale, glm::quat const& rot, glm::vec4 const& clr);
		static void DrawSprite(glm::vec3 const& pos, glm::vec2 const& scale, glm::quat const& rot, Texture const& tex, glm::vec4 const& tint, int entity = -1);
		static void DrawBox(glm::vec3 const& pos, glm::vec3 const& scale, glm::quat const& rot, glm::vec4 const& clr);
		static void DrawBox(glm::mat4 const& mtx, glm::vec4 const& clr);
		static void DrawWireSphere(glm::vec3 const& pos, float radius, glm::vec4 const& clr);
		static void DrawWireCapsule(glm::mat4 const& transformMtx, float radius, float height, glm::vec4 const& clr);
		static void RenderFullscreenTexture();

		static void SubmitMesh(std::shared_ptr<Mesh> mesh, glm::vec3 const& pos, glm::vec3 const& rot, glm::vec3 const& scale, glm::vec4 const& clr = Color::COLOR_WHITE);
		static void SubmitTriangle(glm::vec3 const& v1, glm::vec3 const& v2, glm::vec3 const& v3, glm::vec4 const& clr = Color::COLOR_WHITE);

		//Instancing
		static void SubmitInstance(IGE::Assets::GUID meshSource, glm::mat4 const& worldMtx, glm::vec4 const& clr, int entityID = -1, int matID = 0);
		static void SubmitSubmeshInstance(IGE::Assets::GUID meshSource, size_t submeshIndex, glm::mat4 const& worldMtx, glm::vec4 const& clr, int id, int matID);
		static void RenderInstances();

		static void RenderSubmeshInstances();

		// Batching
		static void BeginBatch();
		static void FlushBatch();
		static void FlushBatch(std::shared_ptr<RenderPass> const& renderPass);

		static void RenderSceneBegin(glm::mat4 const& viewProjMtx);
		static void RenderSceneEnd();

		static unsigned int GetMaxTextureUnits();
		static std::shared_ptr<Graphics::Framebuffer> GetFinalFramebuffer();
		static void SetFinalFramebuffer(std::shared_ptr<Graphics::Framebuffer> const& framebuffer);
		static IGE::Assets::GUID GetDefaultTexture();
		static IGE::Assets::GUID GetWhiteTexture();

		static IGE::Assets::GUID GetDebugMeshSource(size_t idx = 0);

		static IGE::Assets::GUID GetQuadMeshSource();
		

		
	private:
		static void SetQuadBufferData(glm::vec3 const& pos, glm::vec4 const& clr,
									  glm::vec2 const& texCoord, float texIdx, int entity);

		//static void SetQuadBufferData(glm::vec3 const& pos, glm::vec2 const& scale, float texIdx, glm::vec4 const& clr);

		static void SetTriangleBufferData(glm::vec3 const& pos, glm::vec4 const& clr);

		static void SetLineBufferData(glm::vec3 const& pos, glm::vec4 const& clr);

		static void SetMeshBufferData(glm::vec3 const& pos, glm::vec3 const& norm,
			glm::vec2 const& texCoord, float texIdx,
			glm::vec3 const& tangent, glm::vec3 const& bitangent,
			glm::vec4 const& clr);

		static std::shared_ptr<VertexBuffer> GetInstanceBuffer(IGE::Assets::GUID const& meshSrc);
		static std::shared_ptr<VertexBuffer> GetSubmeshInstanceBuffer(MeshSubmeshKey const& meshSubmeshKey);
		//static void SetQuadBufferData(const glm::vec3& pos, const glm::vec2& scale,
		//	const glm::vec4& clr, const glm::vec2& texCoord, float texIdx, int entity);
		static void NextBatch();

		// Stats
		static Statistics GetStats();
		static void ResetStats();

		static void InitShaders();
		static void InitPickPass();
		static void InitGeomPass();
		static void InitShadowMapPass();
		static void InitScreenPass();
		static void InitPostProcessPass();
		static void InitUIPass();
		static void InitMeshSources();

		static void InitFullscreenQuad();

		static void InitUICamera();

		template <typename T>
		static void AddPass(std::shared_ptr<T>&& pass) {
			mTypeToRenderPass.emplace(typeid(T), pass);
			mRenderPasses.emplace_back(std::move(pass));
		}

	private:
		static RendererData mData;
		static MaterialTable mMaterialTable;
		static ShaderLibrary mShaderLibrary;
		static std::shared_ptr<Framebuffer> mFinalFramebuffer;

	public: // TEMP
		template <typename T>
		static std::shared_ptr<T> GetPass() { return std::static_pointer_cast<T>(mTypeToRenderPass[typeid(T)]); }

		static std::unordered_map<std::type_index, std::shared_ptr<RenderPass>> mTypeToRenderPass;
		static std::vector<std::shared_ptr<RenderPass>> mRenderPasses;
		static Component::Camera mUICamera;
	};

}