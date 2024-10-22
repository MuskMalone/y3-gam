#include <pch.h>
#include "Renderer.h"
#include "RenderAPI.h"
#include "RenderPass.h"
#include <glm/gtx/quaternion.hpp>
#include "Asset/IGEAssets.h"
namespace Graphics {
	constexpr int INVALID_ENTITY_ID = -1;

	RendererData Renderer::mData;
	std::shared_ptr<Framebuffer> Renderer::mFinalFramebuffer;
	std::shared_ptr<RenderPass> Renderer::mPickPass; //TODO put in a map/ vector
	std::shared_ptr<RenderPass> Renderer::mGeomPass;

	void Renderer::Init() {

		mData.maxTexUnits = GetMaxTextureUnits();
		mData.texUnits = std::vector<std::shared_ptr<Texture>>(mData.maxTexUnits);

		// Quads
		mData.quadVertexArray = VertexArray::Create();
		mData.quadVertexBuffer = VertexBuffer::Create(mData.cMaxVertices * sizeof(QuadVtx));

		BufferLayout quadLayout = {
			{AttributeType::VEC3, "a_Position"},
			{AttributeType::VEC3, "a_Normal"},
			{AttributeType::VEC2, "a_TexCoord"},
			{AttributeType::FLOAT, "a_TexIdx"},
			{AttributeType::VEC3, "a_Tangent"},
			{AttributeType::VEC3, "a_Bitangent"},
			{AttributeType::VEC4, "a_Color"},
		};

		mData.quadVertexBuffer->SetLayout(quadLayout);
		mData.quadVertexArray->AddVertexBuffer(mData.quadVertexBuffer);
		mData.quadBuffer = std::vector<QuadVtx>(mData.cMaxVertices);

		std::vector<unsigned int> quadIndices(mData.cMaxIndices);

		unsigned int offset{};
		for (size_t i{}; i < mData.cMaxIndices; i += 6) {
			quadIndices[i + 0] = offset + 0;
			quadIndices[i + 1] = offset + 1;
			quadIndices[i + 2] = offset + 2;

			quadIndices[i + 3] = offset + 2;
			quadIndices[i + 4] = offset + 3;
			quadIndices[i + 5] = offset + 0;

			offset += 4;
		}

		std::shared_ptr<ElementBuffer> quadEbo = ElementBuffer::Create(quadIndices.data(), mData.cMaxIndices);
		mData.quadVertexArray->SetElementBuffer(quadEbo);

		//Meshes
		mData.meshVertexArray = VertexArray::Create();
		mData.meshVertexBuffer = VertexBuffer::Create(mData.cMaxVertices * sizeof(Vertex));

		BufferLayout vertexMeshLayout = {
			{AttributeType::VEC3, "a_Position"},
			{AttributeType::VEC3, "a_Normal"},
			{AttributeType::VEC2, "a_TexCoord"},
			{AttributeType::FLOAT, "a_TexIdx"},
			{AttributeType::VEC3, "a_Tangent"},
			{AttributeType::VEC3, "a_Bitangent"},
			{AttributeType::VEC4, "a_Color"},
		};

		mData.meshVertexBuffer->SetLayout(vertexMeshLayout);
		mData.meshVertexArray->AddVertexBuffer(mData.meshVertexBuffer);
		mData.meshBuffer = std::vector<Vertex>(mData.cMaxVertices);

		std::shared_ptr<ElementBuffer> meshEbo = ElementBuffer::Create(mData.cMaxIndices);
		mData.meshVertexArray->SetElementBuffer(meshEbo);

		//====================================================================

		//Triangles
		mData.triVertexArray = VertexArray::Create();
		mData.triVertexBuffer = VertexBuffer::Create(mData.cMaxVertices * sizeof(TriVtx));

		BufferLayout triLayout = {
			{AttributeType::VEC3, "a_Position"},
			{AttributeType::VEC4, "a_Color"},
		};

		mData.triVertexBuffer->SetLayout(triLayout);
		mData.triVertexArray->AddVertexBuffer(mData.triVertexBuffer);
		mData.triBuffer = std::vector<TriVtx>(mData.cMaxVertices);

		//========================================================
		mData.defaultTex = IGE_ASSETMGR.LoadRef<IGE::Assets::TextureAsset>(gAssetsDirectory + std::string("Textures/default.dds"));
		mData.whiteTex = IGE_ASSETMGR.LoadRef<IGE::Assets::TextureAsset>(gAssetsDirectory + std::string("Textures/white.dds"));
		//unsigned int whiteTexData{ 0xffffffff };
		//mData.whiteTex->SetData(&whiteTexData);
		//mData.texUnits[0] = mData.whiteTex;

		std::vector<int> samplers(mData.maxTexUnits);
		for (unsigned int i{}; i < mData.maxTexUnits; ++i)
			samplers[i] = i;

		mData.lineShader = std::make_shared<Shader>("../Assets/Shaders/Tri.vert.glsl", "../Assets/Shaders/Tri.frag.glsl");
		mData.texShader = std::make_shared<Shader>("../Assets/Shaders/Default.vert.glsl", "../Assets/Shaders/Default.frag.glsl");
		mData.texShader->Use();
		mData.texShader->SetUniform("u_Tex", samplers.data(), mData.maxTexUnits);

		mData.quadVtxPos[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
		mData.quadVtxPos[1] = { 0.5f, -0.5f, 0.0f, 1.0f };
		mData.quadVtxPos[2] = { 0.5f,  0.5f, 0.0f, 1.0f };
		mData.quadVtxPos[3] = { -0.5f, 0.5f, 0.0f, 1.0f };

		mData.instancedShader = Shader::Create("../Assets/Shaders/Instanced.vert.glsl", "../Assets/Shaders/Instanced.frag.glsl");

		//Init framebuffer
		Graphics::FramebufferSpec framebufferSpec;
		framebufferSpec.width = WINDOW_WIDTH<int>;
		framebufferSpec.height = WINDOW_HEIGHT<int>;
		framebufferSpec.attachments = { Graphics::FramebufferTextureFormat::RGBA8, Graphics::FramebufferTextureFormat::RED_INTEGER, Graphics::FramebufferTextureFormat::DEPTH };

		//Init RenderPasses
		PipelineSpec geomPipelineSpec;
		geomPipelineSpec.shader = mData.instancedShader;
		geomPipelineSpec.targetFramebuffer = Framebuffer::Create(framebufferSpec);

		RenderPassSpec geomPassSpec;
		geomPassSpec.pipeline = Pipeline::Create(geomPipelineSpec);
		geomPassSpec.debugName = "Geometry Pass";

		//Init PickingPass
		Graphics::FramebufferSpec pickBufferSpec;
		pickBufferSpec.width = WINDOW_WIDTH<int>;
		pickBufferSpec.height = WINDOW_HEIGHT<int>;
		pickBufferSpec.attachments = { Graphics::FramebufferTextureFormat::RED_INTEGER, Graphics::FramebufferTextureFormat::DEPTH };
		
		PipelineSpec pickPipelineSpec;
		pickPipelineSpec.shader = Shader::Create("../Assets/Shaders/Instanced.vert.glsl", "../Assets/Shaders/Instanced.frag.glsl");
		pickPipelineSpec.targetFramebuffer = Framebuffer::Create(pickBufferSpec);

		RenderPassSpec pickPassSpec;
		pickPassSpec.pipeline = Pipeline::Create(pickPipelineSpec);
		pickPassSpec.debugName = "Picking Pass";

		mPickPass = RenderPass::Create(pickPassSpec);
		mGeomPass = RenderPass::Create(geomPassSpec);

		mFinalFramebuffer = mGeomPass->GetTargetFramebuffer();
		//mFinalFramebuffer = Framebuffer::Create(framebufferSpec);


		IGE::Assets::GUID texguid1 { Graphics::Texture::Create(gAssetsDirectory + std::string("Textures\\ogre_normalmap.dds")) };

		IGE::Assets::GUID texguid { Graphics::Texture::Create(gAssetsDirectory + std::string("Textures\\happy.dds")) };
		//unsigned int data[4] = {
		//	0xffff00ff, // Bright magenta (ABGR)
		//	0xffffff00, // Cyan (to create contrast for checkerboard)
		//	0xffffff00, // Cyan
		//	0xffff00ff  // Bright magenta
		//};
		//debugAlbedoTex->SetData(data);



		//TESTING
		//albedo
		mData.albedoMaps.push_back(GetDefaultTexture());
		mData.albedoMaps.push_back(GetWhiteTexture());
		mData.albedoMaps.push_back(texguid1);

		//normal
		mData.normalMaps.push_back(GetWhiteTexture());
		mData.normalMaps.push_back(GetWhiteTexture());
		mData.normalMaps.push_back(texguid1);
		
		//mData.albedoMaps.push_back(texguid1);

		mData.test = std::make_shared<Texture>(2, 2, true);
		// Define a 2x2 checkerboard pattern with magenta and cyan in ABGR format
		unsigned int data[4] = {
			0xffff00ff,  // Magenta (top-left)
			0xffffff00,  // Cyan (top-right)
			0xffffff00,  // Cyan (bottom-left)
			0xffff00ff   // Magenta (bottom-right)
		};
		mData.test->SetData(data);
;	}


	void Renderer::Shutdown() {
		// Add shutdown logic if necessary
	}

	void Renderer::SetQuadBufferData(glm::vec3 const& pos, glm::vec2 const& scale, glm::vec3 const& norm, glm::vec2 const& texCoord, float texIdx, glm::vec3 const& tangent, glm::vec3 const& bitangent, glm::vec4 const& clr) {
		if (mData.quadBufferIndex < mData.quadBuffer.size()) {
			QuadVtx& vtx = mData.quadBuffer[mData.quadBufferIndex];
			vtx.pos = pos;
			vtx.normal = norm;
			vtx.texCoord = texCoord;
			vtx.texIdx = texIdx;
			vtx.tangent = tangent;
			vtx.bitangent = bitangent;
			vtx.clr = clr;
		}
		++mData.quadBufferIndex;
	}

	void Renderer::SetTriangleBufferData(glm::vec3 const& pos, glm::vec4 const& clr) {
		if (mData.triVtxCount < mData.triBuffer.size()) {
			TriVtx& vtx = mData.triBuffer[mData.triBufferIndex];
			vtx.pos = pos;
			vtx.clr = clr;
		}
		++mData.triBufferIndex;
		++mData.triVtxCount;
	}

	void Renderer::SetMeshBufferData(glm::vec3 const& pos, glm::vec3 const& norm, glm::vec2 const& texCoord, float texIdx, glm::vec3 const& tangent, glm::vec3 const& bitangent, glm::vec4 const& clr) {
		if (mData.meshVtxCount < mData.meshBuffer.size()) {
			Vertex& vtx = mData.meshBuffer[mData.meshVtxCount];
			vtx.position = pos;
			vtx.normal = norm;
			vtx.texcoord = texCoord;
			//vtx.texIdx = texIdx;
			vtx.tangent = tangent;
			vtx.bitangent = bitangent;
			vtx.color = clr;
		}
		++mData.meshVtxCount;
	}

	std::shared_ptr<VertexBuffer> Renderer::GetInstanceBuffer(IGE::Assets::GUID const& meshSrc) {
		auto it = mData.instanceBuffers.find(meshSrc); // check if instance buff alr exists

		// found
		if (it != mData.instanceBuffers.end()) {
			return it->second; //return instance buffer
		}

		// else creatae new instance buffer
		uint32_t instanceCap = 100;
		auto instanceBuffer = VertexBuffer::Create(instanceCap * sizeof(InstanceData));

		// Set up the buffer layout for instance data
		BufferLayout instanceLayout = {
			{ AttributeType::MAT4, "a_ModelMatrix" },
			{ AttributeType::INT, "a_MaterialIdx"},
			{ AttributeType::UVEC2, "a_AlbedoHandle"},
			{ AttributeType::INT, "a_EntityID"}
			//{ AttributeType::VEC4, "a_Color" }
		};

		instanceBuffer->SetLayout(instanceLayout);

		// Attach the instance buffer to the MeshSource's VAO
		IGE_REF(IGE::Assets::MeshAsset, meshSrc)->mMeshSource.GetVertexArray()->AddVertexBuffer(instanceBuffer, true);

		// Store the buffer in the map for future use
		mData.instanceBuffers[meshSrc] = instanceBuffer;

		// Return the new instance buffer
		return instanceBuffer;
	}

	void Renderer::DrawQuad(glm::vec3 const& pos, glm::vec2 const& scale, glm::vec4 const& clr, float rot) {
		if (mData.quadIdxCount >= RendererData::cMaxIndices)
			NextBatch();

		constexpr glm::vec2 texCoords[4]{ { 0.f, 0.f }, {1.f, 0.f}, {1.f, 1.f}, {0.f, 1.f} };
		const float texIdx{}; // white tex index = 0

		glm::mat4 translateMtx{ glm::translate(glm::mat4{ 1.f }, pos) };
		glm::mat4 rotateMtx{ glm::rotate(glm::mat4{ 1.f }, glm::radians(rot), {0.f, 0.f, 1.f}) };
		glm::mat4 scaleMtx{ glm::scale(glm::mat4{ 1.f }, { scale.x, scale.y, 1.f }) };
		glm::mat4 transformMtx{ translateMtx * rotateMtx * scaleMtx };



		// Normal for a quad is typically perpendicular to the surface (pointing along z-axis in local space)
		glm::vec3 normal = { 0.f, 0.f, 1.f };

		// Tangent points in the direction of increasing U (horizontal direction in texture space)
		glm::vec3 tangent = { 1.f, 0.f, 0.f };

		// Bitangent points in the direction of increasing V (vertical direction in texture space)
		glm::vec3 bitangent = { 0.f, 1.f, 0.f };

		for (size_t i{}; i < 4; ++i)
			SetQuadBufferData(transformMtx * mData.quadVtxPos[i],scale, normal, texCoords[i], texIdx, tangent, bitangent, clr);

		mData.quadIdxCount += 6;
		++mData.stats.quadCount;
	}

	void Renderer::SubmitMesh(std::shared_ptr<Mesh> mesh, glm::vec3 const& pos, glm::vec3 const& rot, glm::vec3 const& scale, glm::vec4 const& clr) {
		if (mesh == nullptr) return;
		auto const& meshSrc = mesh->GetMeshSource();
		auto const& submeshes = IGE_REF(IGE::Assets::MeshAsset, meshSrc)->mMeshSource.GetSubmeshes();

		// Transformation matrices
		glm::mat4 translateMtx{ glm::translate(glm::mat4{ 1.f }, pos) };
		//glm::mat4 rotateMtx{ glm::rotate(glm::mat4{ 1.f }, glm::radians(rot), {0.f, 1.f, 0.f}) };
		glm::mat4 rotateMtx = glm::mat4(1.f);
		glm::mat4 scaleMtx{ glm::scale(glm::mat4{ 1.f }, scale) };
		glm::mat4 transformMtx{ translateMtx * rotateMtx * scaleMtx };

		// Normal matrix (3x3 portion of the model matrix)
		glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(transformMtx)));

		// Iterate over the submeshes
		for (const auto& submesh : submeshes) {
			// Check if adding this submesh would exceed the batch capacity
			if (mData.meshIdxCount + submesh.idxCount >= RendererData::cMaxIndices ||
				mData.meshVtxCount + submesh.vtxCount >= RendererData::cMaxVertices) {
				// Flush the current batch before adding the new submesh
				NextBatch();
			}

			// Apply the instance's transformation to the submesh's transform
			glm::mat4 finalxformMtx = transformMtx/* * submesh.transform*/;

			//// Collect index data from the submesh
			//for (uint32_t i = 0; i < submesh.idxCount; ++i) {
			//	mData.meshIdxBuffer.push_back(submesh.baseIdx + i + mData.meshVtxCount);
			//}
			for (const auto& idx : submesh.mIndices) {
				// Adjust index by the current vertex count to maintain correct offsets
				mData.meshIdxBuffer.push_back(idx + mData.meshVtxCount);
			}

			// Collect vertex data from the submesh
			for (size_t i = 0; i < submesh.vtxCount; ++i) {
				const Vertex& vtx = IGE_REF(IGE::Assets::MeshAsset, meshSrc)->mMeshSource.GetVertices()[submesh.baseVtx + i];

				// Transform position to world space
				glm::vec3 worldPos = glm::vec3(finalxformMtx * glm::vec4(vtx.position, 1.0f));

				// Transform normal, tangent, and bitangent to world space using the normal matrix
				glm::vec3 worldNormal = glm::normalize(normalMatrix * vtx.normal);
				glm::vec3 worldTangent = glm::normalize(normalMatrix * vtx.tangent);
				glm::vec3 worldBitangent = glm::normalize(normalMatrix * vtx.bitangent);

				// Map vertex data into the batch buffer
				SetMeshBufferData(worldPos, worldNormal, vtx.texcoord, 0.f, worldTangent, worldBitangent, clr);
			}

			//mData.meshVtxCount += submesh.vtxCount;
			mData.meshIdxCount += submesh.idxCount;
		}
	}

	void Renderer::SubmitTriangle(glm::vec3 const& v1, glm::vec3 const& v2, glm::vec3 const& v3, glm::vec4 const& clr) {
		if (mData.triVtxCount >= RendererData::cMaxVertices)
			NextBatch();

		SetTriangleBufferData(v1, clr);
		SetTriangleBufferData(v2, clr);
		SetTriangleBufferData(v3, clr);
	}

	void Renderer::SubmitInstance(std::shared_ptr<Mesh> mesh, glm::mat4 const& worldMtx, glm::vec4 const& clr, uint64_t albedoHdl, int id) {
		if (!mesh) return;

		InstanceData instance{};
		instance.modelMatrix = worldMtx;

		instance.albedo[0] = static_cast<uint32_t>(albedoHdl & 0xFFFFFFFF); // Lower 32 bits
		instance.albedo[1] = static_cast<uint32_t>(albedoHdl >> 32); // Upper 32 bits

		
		if (id != INVALID_ENTITY_ID) {
			instance.entityID = id;
		}
		instance.materialIdx = 0;

		auto& meshSrc = mesh->GetMeshSource();
		if (!meshSrc) return;

		mData.instanceBufferDataMap[meshSrc].push_back(instance);
	}


	void Renderer::RenderInstances() {
		for (auto& [meshSrc, instances] : mData.instanceBufferDataMap) {
			if (instances.empty()) continue;

			auto instanceBuffer = GetInstanceBuffer(meshSrc);
			//instanceBuffer->SetLayout();

			// Set instance data into the buffer
			unsigned int dataSize = static_cast<unsigned int>(instances.size() * sizeof(InstanceData));
			
			instanceBuffer->SetData(instances.data(), dataSize);

			// Bind the VAO and render the instances
			auto& vao = IGE_REF(IGE::Assets::MeshAsset, meshSrc)->mMeshSource.GetVertexArray();

			RenderAPI::DrawIndicesInstanced(vao, static_cast<unsigned>(IGE_REF(IGE::Assets::MeshAsset, meshSrc)->mMeshSource.GetIndices().size()), static_cast<unsigned>(instances.size()));

		}

		mData.instanceBufferDataMap.clear();
	}

	void Renderer::FlushBatch() {
		if (mData.quadIdxCount) {
			//ptrdiff_t difference{ reinterpret_cast<unsigned char*>(mData.quadBufferPtr)
			//					- reinterpret_cast<unsigned char*>(mData.quadBuffer.data()) };

			//how many elements it takes up in terms of bytes
			//unsigned int dataSize = static_cast<unsigned int>(difference);

			unsigned int dataSize = static_cast<unsigned int>(mData.quadBufferIndex * sizeof(QuadVtx));

			mData.quadVertexBuffer->SetData(mData.quadBuffer.data(), dataSize);

			//Bind all the textures that has been set
			for (unsigned int i{}; i < mData.texUnitIdx; ++i) {
				mData.texUnits[i]->Bind(i);
			}
			mData.texShader->Use();
			RenderAPI::DrawIndices(mData.quadVertexArray, mData.quadIdxCount);

			++mData.stats.drawCalls;
		}
		if (mData.triVtxCount) {

			unsigned int dataSize = static_cast<unsigned int>(mData.triBufferIndex * sizeof(TriVtx));

			mData.triVertexBuffer->SetData(mData.triBuffer.data(), dataSize);

			mData.lineShader->Use();
			RenderAPI::DrawLines(mData.triVertexArray, mData.triVtxCount);

			++mData.stats.drawCalls;
		}
		if (mData.meshIdxCount) {

			// Calculate data size for mesh vertices
			unsigned int dataSize = static_cast<unsigned int>(mData.meshVtxCount * sizeof(Vertex));

			// Update the mesh vertex buffer with the batched data
			mData.meshVertexBuffer->SetData(mData.meshBuffer.data(), dataSize);

 			unsigned int idxDataSize = static_cast<unsigned int>(mData.meshIdxCount * sizeof(uint32_t));
			mData.meshVertexArray->Bind();
			mData.meshVertexArray->GetElementBuffer()->SetData(mData.meshIdxBuffer.data(), idxDataSize);
			mData.meshVertexArray->Unbind();
			// Bind the textures for the meshes
			for (unsigned int i{}; i < mData.texUnitIdx; ++i) {
				mData.texUnits[i]->Bind(i);
			}

			// Use the appropriate shader and draw the indexed meshes
			mData.texShader->Use();
			RenderAPI::DrawIndices(mData.meshVertexArray, mData.meshIdxCount);

			// Increment draw call stats
			++mData.stats.drawCalls;
		}

	}

	void Renderer::FlushBatch(std::shared_ptr<RenderPass> const& renderPass) {

		if (mData.triVtxCount) {

			unsigned int dataSize = static_cast<unsigned int>(mData.triBufferIndex * sizeof(TriVtx));

			mData.triVertexBuffer->SetData(mData.triBuffer.data(), dataSize);

			RenderAPI::DrawLines(mData.triVertexArray, mData.triVtxCount);

			++mData.stats.drawCalls;
		}
		if ((renderPass->GetSpecification().debugName) == "Geometry Pass" && mData.meshIdxCount) {

			// Calculate data size for mesh vertices
			unsigned int dataSize = static_cast<unsigned int>(mData.meshVtxCount * sizeof(Vertex));
			mData.meshVertexArray->Bind();
			// Update the mesh vertex buffer with the batched data
			mData.meshVertexBuffer->SetData(reinterpret_cast<void*>(mData.meshBuffer.data()), dataSize);

			unsigned int idxDataSize = static_cast<unsigned int>(mData.meshIdxCount * sizeof(uint32_t));

			mData.meshVertexArray->GetElementBuffer()->SetData(reinterpret_cast<void*>(mData.meshIdxBuffer.data()), idxDataSize);
			//mData.meshVertexArray->Unbind();
			// Bind the textures for the meshes
			for (unsigned int i{}; i < mData.texUnitIdx; ++i) {
				mData.texUnits[i]->Bind(i);
			}

			RenderAPI::DrawIndices(mData.meshVertexArray, mData.meshIdxCount);

			// Increment draw call stats
			++mData.stats.drawCalls;
		}
	}

	void Renderer::BeginBatch() {
		mData.quadIdxCount = 0;
		mData.triVtxCount = 0;
		//mData.quadBufferPtr = mData.quadBuffer.data();
		mData.quadBufferIndex = 0;  // Reset the index for the new batch
		mData.triBufferIndex = 0;
		mData.texUnitIdx = 1;

		// Reset for general meshes
		mData.meshIdxCount = 0;
		mData.meshVtxCount = 0;
		mData.meshIdxBuffer.clear();

	}

	void Renderer::NextBatch() {
		FlushBatch();
		BeginBatch();
	}

	void Renderer::RenderSceneBegin(glm::mat4 const& viewProjMtx) {

		//mData.lineShader->Use();
		//mData.lineShader->SetUniform("u_ViewProjMtx", viewProjMtx);
		//mData.texShader->Use();
		//mData.texShader->SetUniform("u_ViewProjMtx", viewProjMtx);

		BeginBatch();
	}

	/*  _________________________________________________________________________ */
	/*! RenderSceneEnd

	@return none.

	Ends the rendering scene.
	Flushes the batch of primitives to be rendered.
	*/
	void Renderer::RenderSceneEnd() {

		FlushBatch();
	}

	std::shared_ptr<Material> Renderer::GetMaterial(uint32_t idx){
		return mData.materialVector[idx];
	}

	std::vector< IGE::Assets::GUID> const& Renderer::GetAlbedoMaps(){
		return mData.albedoMaps;
	}

	std::vector<IGE::Assets::GUID> const& Renderer::GetNormalMaps() {
		return mData.normalMaps;
	}

	Statistics Renderer::GetStats() {
		return mData.stats;
	}

	void Renderer::ResetStats() {
		memset(&mData.stats, 0, sizeof(Statistics));
	}

	unsigned int Renderer::GetMaxTextureUnits() {
		int maxTexUnits;
		glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxTexUnits);
		return static_cast<unsigned int>(maxTexUnits);
	}

	std::shared_ptr<Graphics::Framebuffer> Renderer::GetFinalFramebuffer() {
		return mFinalFramebuffer; // Assuming `mFinalFramebuffer` holds the final rendered result
	}

	void Renderer::SetFinalFramebuffer(std::shared_ptr<Graphics::Framebuffer> const& framebuffer) {
		mFinalFramebuffer = framebuffer;
	}

	IGE::Assets::GUID Renderer::GetDefaultTexture()
	{
		return mData.defaultTex;
	}

	IGE::Assets::GUID Renderer::GetWhiteTexture() {
		return mData.whiteTex;
	}

	std::shared_ptr<Texture> Renderer::TestGet()
	{
		return mData.test;
	}

}