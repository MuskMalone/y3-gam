#include <pch.h>
#include "Renderer.h"
#include "RenderAPI.h"

namespace Graphics {
	RendererData Renderer::mData;

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

		//Cubes
		mData.cubeVertexArray = VertexArray::Create();
		mData.cubeVertexBuffer = VertexBuffer::Create(mData.cMaxVertices * sizeof(QuadVtx));

		BufferLayout cubeLayout = {
			{AttributeType::VEC3, "a_Position"},
			{AttributeType::VEC3, "a_Normal"},
			{AttributeType::VEC2, "a_TexCoord"},
			{AttributeType::FLOAT, "a_TexIdx"},
			{AttributeType::VEC3, "a_Tangent"},
			{AttributeType::VEC3, "a_Bitangent"},
			{AttributeType::VEC4, "a_Color"},
		};

		mData.cubeVertexBuffer->SetLayout(cubeLayout);
		mData.cubeVertexArray->AddVertexBuffer(mData.cubeVertexBuffer);
		mData.cubeBuffer = std::vector<CubeVtx>(mData.cMaxVertices);

		// Setup for cubes
		std::vector<unsigned int> cubeIndices(mData.cMaxIndices); // 36 indices per cube

		unsigned int cubeOffset{};
		for (size_t i{}; i < mData.cMaxIndices; i += 36) {
			// Front face
			cubeIndices[i + 0] = cubeOffset + 0; // Bottom-left
			cubeIndices[i + 1] = cubeOffset + 1; // Bottom-right
			cubeIndices[i + 2] = cubeOffset + 2; // Top-right
			cubeIndices[i + 3] = cubeOffset + 2; // Top-right
			cubeIndices[i + 4] = cubeOffset + 3; // Top-left
			cubeIndices[i + 5] = cubeOffset + 0; // Bottom-left

			// Back face
			cubeIndices[i + 6] = cubeOffset + 4; // Bottom-right
			cubeIndices[i + 7] = cubeOffset + 5; // Bottom-left
			cubeIndices[i + 8] = cubeOffset + 6; // Top-left
			cubeIndices[i + 9] = cubeOffset + 6; // Top-left
			cubeIndices[i + 10] = cubeOffset + 7; // Top-right
			cubeIndices[i + 11] = cubeOffset + 4; // Bottom-right

			// Left face
			cubeIndices[i + 12] = cubeOffset + 8; // Bottom-back
			cubeIndices[i + 13] = cubeOffset + 9; // Bottom-front
			cubeIndices[i + 14] = cubeOffset + 10; // Top-front
			cubeIndices[i + 15] = cubeOffset + 10; // Top-front
			cubeIndices[i + 16] = cubeOffset + 11; // Top-back
			cubeIndices[i + 17] = cubeOffset + 8; // Bottom-back

			// Right face
			cubeIndices[i + 18] = cubeOffset + 12; // Bottom-front
			cubeIndices[i + 19] = cubeOffset + 13; // Bottom-back
			cubeIndices[i + 20] = cubeOffset + 14; // Top-back
			cubeIndices[i + 21] = cubeOffset + 14; // Top-back
			cubeIndices[i + 22] = cubeOffset + 15; // Top-front
			cubeIndices[i + 23] = cubeOffset + 12; // Bottom-front

			// Top face
			cubeIndices[i + 24] = cubeOffset + 16; // Front-left
			cubeIndices[i + 25] = cubeOffset + 17; // Front-right
			cubeIndices[i + 26] = cubeOffset + 18; // Back-right
			cubeIndices[i + 27] = cubeOffset + 18; // Back-right
			cubeIndices[i + 28] = cubeOffset + 19; // Back-left
			cubeIndices[i + 29] = cubeOffset + 16; // Front-left

			// Bottom face
			cubeIndices[i + 30] = cubeOffset + 20; // Back-left
			cubeIndices[i + 31] = cubeOffset + 21; // Back-right
			cubeIndices[i + 32] = cubeOffset + 22; // Front-right
			cubeIndices[i + 33] = cubeOffset + 22; // Front-right
			cubeIndices[i + 34] = cubeOffset + 23; // Front-left
			cubeIndices[i + 35] = cubeOffset + 20; // Back-left

			cubeOffset += 24; // Each cube has 24 vertices (6 faces, 4 vertices per face)
		}

		std::shared_ptr<ElementBuffer> cubeEbo = ElementBuffer::Create(cubeIndices.data(), mData.cMaxIndices);
		mData.cubeVertexArray->SetElementBuffer(cubeEbo);

		//========================================================

		mData.whiteTex = std::make_shared<Texture>(1, 1);
		unsigned int whiteTexData{ 0xffffffff };
		mData.whiteTex->SetData(&whiteTexData);
		mData.texUnits[0] = mData.whiteTex;

		std::vector<int> samplers(mData.maxTexUnits);
		for (unsigned int i{}; i < mData.maxTexUnits; ++i)
			samplers[i] = i;

		mData.texShader = std::make_shared<Shader>("./assets/Shaders/Default.vert.glsl", "./assets/Shaders/Default.frag.glsl");
		mData.texShader->Use();
		mData.texShader->SetUniform("u_Tex", samplers.data(), mData.maxTexUnits);

		mData.quadVtxPos[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
		mData.quadVtxPos[1] = { 0.5f, -0.5f, 0.0f, 1.0f };
		mData.quadVtxPos[2] = { 0.5f,  0.5f, 0.0f, 1.0f };
		mData.quadVtxPos[3] = { -0.5f, 0.5f, 0.0f, 1.0f };

		mData.cubeVtxPos = {
			// Front face (z = +0.5)
			glm::vec4{-0.5f, -0.5f,  0.5f, 1.0f}, // Bottom-left
			glm::vec4{ 0.5f, -0.5f,  0.5f, 1.0f}, // Bottom-right
			glm::vec4{ 0.5f,  0.5f,  0.5f, 1.0f}, // Top-right
			glm::vec4{-0.5f,  0.5f,  0.5f, 1.0f}, // Top-left

			// Back face (z = -0.5)
			glm::vec4{ 0.5f, -0.5f, -0.5f, 1.0f}, // Bottom-right
			glm::vec4{-0.5f, -0.5f, -0.5f, 1.0f}, // Bottom-left
			glm::vec4{-0.5f,  0.5f, -0.5f, 1.0f}, // Top-left
			glm::vec4{ 0.5f,  0.5f, -0.5f, 1.0f}, // Top-right

			// Left face (x = -0.5)
			glm::vec4{-0.5f, -0.5f, -0.5f, 1.0f}, // Bottom-back
			glm::vec4{-0.5f, -0.5f,  0.5f, 1.0f}, // Bottom-front
			glm::vec4{-0.5f,  0.5f,  0.5f, 1.0f}, // Top-front
			glm::vec4{-0.5f,  0.5f, -0.5f, 1.0f}, // Top-back

			// Right face (x = +0.5)
			glm::vec4{ 0.5f, -0.5f,  0.5f, 1.0f}, // Bottom-front
			glm::vec4{ 0.5f, -0.5f, -0.5f, 1.0f}, // Bottom-back
			glm::vec4{ 0.5f,  0.5f, -0.5f, 1.0f}, // Top-back
			glm::vec4{ 0.5f,  0.5f,  0.5f, 1.0f}, // Top-front

			// Top face (y = +0.5)
			glm::vec4{-0.5f,  0.5f,  0.5f, 1.0f}, // Front-left
			glm::vec4{ 0.5f,  0.5f,  0.5f, 1.0f}, // Front-right
			glm::vec4{ 0.5f,  0.5f, -0.5f, 1.0f}, // Back-right
			glm::vec4{-0.5f,  0.5f, -0.5f, 1.0f}, // Back-left	

			// Bottom face (y = -0.5)
			glm::vec4{-0.5f, -0.5f, -0.5f, 1.0f}, // Back-left
			glm::vec4{ 0.5f, -0.5f, -0.5f, 1.0f}, // Back-right
			glm::vec4{ 0.5f, -0.5f,  0.5f, 1.0f}, // Front-right
			glm::vec4{-0.5f, -0.5f,  0.5f, 1.0f}, // Front-left
		};


	}

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

	void Renderer::SetCubeBufferData(glm::vec3 const& pos, glm::vec2 const& scale, glm::vec3 const& norm, glm::vec2 const& texCoord, float texIdx, glm::vec3 const& tangent, glm::vec3 const& bitangent, glm::vec4 const& clr) {
		if (mData.cubeBufferIndex < mData.cubeBuffer.size()) {
			CubeVtx& vtx = mData.cubeBuffer[mData.cubeBufferIndex];
			vtx.pos = pos;
			vtx.normal = norm;
			vtx.texCoord = texCoord;
			vtx.texIdx = texIdx;
			vtx.tangent = tangent;
			vtx.bitangent = bitangent;
			vtx.clr = clr;
		}
		++mData.cubeBufferIndex;
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

	void Renderer::SubmitCube(glm::vec3 const& pos, glm::vec3 const& scale, glm::vec4 const& clr, float rot) {
		if (mData.cubeIdxCount >= RendererData::cMaxIndices)
			NextBatch();

		// Define texture coordinates (same for all faces)
		constexpr glm::vec2 texCoords[4]{ { 0.f, 0.f }, {1.f, 0.f}, {1.f, 1.f}, {0.f, 1.f} };

		// Transformation matrices
		glm::mat4 translateMtx{ glm::translate(glm::mat4{ 1.f }, pos) };
		glm::mat4 rotateMtx{ glm::rotate(glm::mat4{ 1.f }, glm::radians(rot), {0.f, 1.f, 0.f}) };
		glm::mat4 scaleMtx{ glm::scale(glm::mat4{ 1.f }, scale) };
		glm::mat4 transformMtx{ translateMtx * rotateMtx * scaleMtx };

		// Front Face (positive Z)
		glm::vec3 frontNormal = { 0.f, 0.f, 1.f };
		glm::vec3 frontTangent = { 1.f, 0.f, 0.f };
		glm::vec3 frontBitangent = { 0.f, 1.f, 0.f };
		for (size_t i = 0; i < 24; ++i) {
			SetCubeBufferData(transformMtx * mData.cubeVtxPos[i], scale, frontNormal, texCoords[i], 0.f, frontTangent, frontBitangent, clr);
		}

		mData.cubeIdxCount += 36; // 6 faces * 6 indices per face
		//++mData.stats.quadCount;
	}

	void Renderer::SubmitMesh(std::shared_ptr<Mesh> mesh, glm::vec3 const& pos, glm::vec3 const& scale, glm::vec4 const& clr, float rot) {
		auto const& meshSrc = mesh->GetMeshSource();
		auto const& submeshes = meshSrc->GetSubmeshes();

		// Transformation matrices
		glm::mat4 translateMtx{ glm::translate(glm::mat4{ 1.f }, pos) };
		glm::mat4 rotateMtx{ glm::rotate(glm::mat4{ 1.f }, glm::radians(rot), {0.f, 1.f, 0.f}) };
		glm::mat4 scaleMtx{ glm::scale(glm::mat4{ 1.f }, scale) };
		glm::mat4 transformMtx{ translateMtx * rotateMtx * scaleMtx };

		// Iterate over the submeshes
		for (const auto& submesh : submeshes) {
			if (mData.meshIdxCount >= RendererData::cMaxIndices) {
				NextBatch();  // Flush if the batch is full
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
				const Vertex& vtx = meshSrc->GetVertices()[submesh.baseVtx + i];

				// Map vertex data into the batch buffer
				SetMeshBufferData(finalxformMtx * glm::vec4(vtx.position, 1.0f), vtx.normal, vtx.texcoord, 0.f, vtx.tangent, vtx.bitangent, clr);
			}

			//mData.meshVtxCount += submesh.vtxCount;
			mData.meshIdxCount += submesh.idxCount;
		}
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
		if (mData.cubeIdxCount) {

			unsigned int dataSize = static_cast<unsigned int>(mData.cubeBufferIndex * sizeof(CubeVtx));

			mData.cubeVertexBuffer->SetData(mData.cubeBuffer.data(), dataSize);

			//Bind all the textures that has been set
			for (unsigned int i{}; i < mData.texUnitIdx; ++i) {
				mData.texUnits[i]->Bind(i);
			}
			mData.texShader->Use();
			RenderAPI::DrawIndices(mData.cubeVertexArray, mData.cubeIdxCount);

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

	void Renderer::BeginBatch() {
		mData.quadIdxCount = 0;
		mData.cubeIdxCount = 0;
		//mData.quadBufferPtr = mData.quadBuffer.data();
		mData.quadBufferIndex = 0;  // Reset the index for the new batch
		mData.cubeBufferIndex = 0;
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

		mData.texShader->Use();
		mData.texShader->SetUniform("u_ViewProjMtx", viewProjMtx);

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

}