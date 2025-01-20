#include <pch.h>
#include "Renderer.h"
#include "RenderAPI.h"
#include <glm/gtx/quaternion.hpp>
#include "Asset/IGEAssets.h"
#include "ElementBuffer.h"
#include "MaterialTable.h"
#include "MaterialData.h"
#include <Graphics/Mesh/Mesh.h>
#include "Events/EventManager.h"
#include "Events/Event.h"

#pragma region RenderPasses
#include <Graphics/RenderPass/SkyboxPass.h>
#include <Graphics/RenderPass/GeomPass.h>
#include <Graphics/RenderPass/ShadowPass.h>
#include <Graphics/RenderPass/ScreenPass.h>
#include <Graphics/RenderPass/PostProcessPass.h>
#include <Graphics/RenderPass/ParticlePass.h>
#include <Graphics/RenderPass/UIPass.h>
#pragma endregion

#include "Core/Components/Camera.h"
#include "Core/Components/Transform.h"
#include "Core/Components/Light.h"
#include "Core/Entity.h"
#include "Input/InputManager.h"

namespace Graphics {
	constexpr int INVALID_ENTITY_ID = -1;

	ECS::Entity Renderer::mHighlightedEntity;
	RendererData Renderer::mData;
	MaterialTable Renderer::mMaterialTable;
	ShaderLibrary Renderer::mShaderLibrary;
	std::vector<IGE::Assets::GUID> Renderer::mIcons;
	std::shared_ptr<Framebuffer> Renderer::mFinalFramebuffer;
	std::unordered_map<std::type_index, std::shared_ptr<RenderPass>> Renderer::mTypeToRenderPass;
	std::vector<std::shared_ptr<RenderPass>> Renderer::mRenderPasses;
	Component::Camera Renderer::mUICamera;

	Renderer::Renderer() {

	}
	void Renderer::Init() {
		SUBSCRIBE_STATIC_FUNC(Events::WindowResized, OnResize);
		SUBSCRIBE_STATIC_FUNC(Events::EntitySelectedInEditor, OnEntityPicked);
		InitUICamera();

		//----------------------Init Batching Quads------------------------------------------------------------//
		mData.quadVertexArray = VertexArray::Create();
		mData.quadVertexBuffer = VertexBuffer::Create(mData.cMaxVertices2D * sizeof(QuadVtx));

		BufferLayout quadLayout = {
			{AttributeType::VEC3, "a_Position"},
			{AttributeType::VEC4, "a_Color"},
			{AttributeType::VEC2, "a_TexCoord"},
			{AttributeType::FLOAT, "a_TexIdx"},
			{AttributeType::INT, "a_Entity"}
		};

		mData.quadVertexBuffer->SetLayout(quadLayout);
		mData.quadVertexArray->AddVertexBuffer(mData.quadVertexBuffer);
		mData.quadBuffer = std::vector<QuadVtx>(mData.cMaxVertices2D);

		std::vector<unsigned int> quadIndices(mData.cMaxIndices2D);

		unsigned int offset{};
		for (size_t i{}; i < mData.cMaxIndices2D; i += 6) {
			quadIndices[i + 0] = offset + 0;
			quadIndices[i + 1] = offset + 1;
			quadIndices[i + 2] = offset + 2;

			quadIndices[i + 3] = offset + 2;
			quadIndices[i + 4] = offset + 3;
			quadIndices[i + 5] = offset + 0;

			offset += 4;
		}

		std::shared_ptr<ElementBuffer> quadEbo = ElementBuffer::Create(quadIndices.data(), mData.cMaxIndices2D);
		mData.quadVertexArray->SetElementBuffer(quadEbo);

		//--------------------------------------------------------------------------------------------------------//
	
		//----------------------------------Meshes Batching-------------------------------------------------------//
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

		//----------------------------------------------------------------------------------------------------------//

		//----------------------------------Triangles Batching-------------------------------------------------------//
		mData.triVertexArray = VertexArray::Create();
		mData.triVertexBuffer = VertexBuffer::Create(mData.cMaxVertices * sizeof(LineVtx));

		BufferLayout lineLayout = { //same layout for tri and line
			{AttributeType::VEC3, "a_Position"},
			{AttributeType::VEC4, "a_Color"},
		};

		mData.triVertexBuffer->SetLayout(lineLayout);
		mData.triVertexArray->AddVertexBuffer(mData.triVertexBuffer);
		mData.triBuffer = std::vector<LineVtx>(mData.cMaxVertices);

		//----------------------------------------------------------------------------------------------------------//

		//----------------------------------Line Batching-------------------------------------------------------//
		mData.lineVertexArray = VertexArray::Create();
		mData.lineVertexBuffer = VertexBuffer::Create(mData.cMaxVertices2D * sizeof(LineVtx));

		mData.lineVertexBuffer->SetLayout(lineLayout);
		mData.lineVertexArray->AddVertexBuffer(mData.lineVertexBuffer);
		mData.lineBuffer = std::vector<LineVtx>(mData.cMaxVertices2D);

		//----------------------------------------------------------------------------------------------------------//
		
		//mData.defaultTex = IGE_ASSETMGR.LoadRef<IGE::Assets::TextureAsset>(gAssetsDirectory + std::string("Texturesdefault.dds"));
		mData.defaultTex = IGE_ASSETMGR.LoadRef<IGE::Assets::TextureAsset>(366429001515961616);
		//mData.whiteTex = IGE_ASSETMGR.LoadRef<IGE::Assets::TextureAsset>(gAssetsDirectory + std::string("Textures/white.dds"));
		mData.whiteTex = IGE_ASSETMGR.LoadRef<IGE::Assets::TextureAsset>(2203736300003804932);
		//unsigned int whiteTexData{ 0xffffffff };
		//mData.whiteTex->SetData(&whiteTexData);
		//mData.texUnits[0] = mData.whiteTex;
		InitMeshSources();

		InitShaders();
		std::shared_ptr<Shader> const& texShader = ShaderLibrary::Get("Tex2D");


		mData.maxTexUnits = GetMaxTextureUnits();
		mData.texUnits = std::vector<Texture>(mData.maxTexUnits);

		mData.texUnits[0] = IGE_ASSETMGR.GetAsset<IGE::Assets::TextureAsset>(mData.whiteTex)->mTexture;

		std::vector<int> samplers(mData.maxTexUnits);
		for (unsigned int i{}; i < mData.maxTexUnits; ++i)
			samplers[i] = i;

		texShader->Use();
		texShader->SetUniform("u_Tex", samplers.data(), mData.maxTexUnits);

		mData.quadVtxPos[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
		mData.quadVtxPos[1] = { 0.5f, -0.5f, 0.0f, 1.0f };
		mData.quadVtxPos[2] = { 0.5f,  0.5f, 0.0f, 1.0f };
		mData.quadVtxPos[3] = { -0.5f, 0.5f, 0.0f, 1.0f };

		//Init framebuffer
		Graphics::FramebufferSpec framebufferSpec;
		framebufferSpec.width = WINDOW_WIDTH<int>;
		framebufferSpec.height = WINDOW_HEIGHT<int>;
		framebufferSpec.attachments = { Graphics::FramebufferTextureFormat::RGBA8, Graphics::FramebufferTextureFormat::RED_INTEGER, Graphics::FramebufferTextureFormat::DEPTH };

		//Init RenderPasses
		//InitPickPass();
		InitShadowMapPass();
		InitGeomPass();
		InitParticlePass();
		InitPostProcessPass();
		InitUIPass();
	
		InitFullscreenQuad();

		mFinalFramebuffer = mRenderPasses.back()->GetTargetFramebuffer();

		IGE::Assets::GUID texguid1{ Texture::Create(gAssetsDirectory + std::string("Textures\\default.dds")) };
		IGE::Assets::GUID texguid{ Texture::Create(gAssetsDirectory + std::string("Textures\\happy.dds")) };
		//Init Materials
		MaterialTable::Init(mData.cMaxMaterials);

		//std::shared_ptr<MaterialData> mat1 = MaterialData::Create("PBR", "MatLighting");
		//mat1->SetAlbedoMap(texguid1);
		//mat1->SetAlbedoColor({ 0.7f,0.6f,0.9f });
		//MaterialTable::AddMaterial(mat1);

		//std::shared_ptr<MaterialData> mat2 = MaterialData::Create("Unlit", "MatNoLight"); //@TODO support other shaders like Unlit
		//mat2->SetAlbedoMap(texguid);
		//MaterialTable::AddMaterial(mat2);
		//--Material Init End--//

		//// Generate multiple materials for testing
		//const int numMaterials = 50; // Total number of materials to create

		//// Random number generator setup
		//std::random_device rd;  // Obtain a random number from hardware
		//std::mt19937 eng(rd()); // Seed the generator
		//std::uniform_real_distribution<> distr(0.0, 1.0); // Define the range for the random numbers

		//for (int i = 0; i < numMaterials; ++i) {
		//	// Create a new material based on the PBR shader
		//	std::shared_ptr<Material> material = Material::Create(ShaderLibrary::Get("PBR"));

		//	// Generate a random albedo color for each material
		//	glm::vec3 albedoColor = glm::vec3(distr(eng), distr(eng), distr(eng)); // Random RGB values

		//	material->SetAlbedoColor(albedoColor);

		//	// Optionally set other properties if needed
		//	material->SetMetalness(0.0f); // Keep metalness constant for this example
		//	material->SetRoughness(distr(eng)); // Vary roughness randomly between 0 and 1

		//	// Add the new material to the material table
		//	MaterialTable::AddMaterial(material);
		//}
		IGE::Assets::GUID sunIcon{ Texture::Create(gAssetsDirectory + std::string("Textures\\sun_icon.png")) };
		IGE::Assets::GUID spotlightIcon{ Texture::Create(gAssetsDirectory + std::string("Textures\\spotlight_icon.png")) };
		//IGE::Assets::GUID cameraIcon{ Texture::Create(gAssetsDirectory + std::string("Textures\\cam_icon.png")) };
		
		mIcons.push_back(IGE_ASSETMGR.LoadRef<IGE::Assets::TextureAsset>(sunIcon));
		mIcons.push_back(IGE_ASSETMGR.LoadRef<IGE::Assets::TextureAsset>(spotlightIcon));
		//mIcons.push_back(IGE_ASSETMGR.LoadRef<IGE::Assets::TextureAsset>(cameraIcon));


	}

	void Renderer::InitShaders() {
		ShaderLibrary::Add("Line", Shader::Create("Line.vert.glsl", "Line.frag.glsl"));
		ShaderLibrary::Add("Tex", Shader::Create("Default.vert.glsl", "Default.frag.glsl"));
		ShaderLibrary::Add("PBR", Shader::Create("PBR.vert.glsl", "PBR.frag.glsl"));
		ShaderLibrary::Add("Unlit", Shader::Create("Unlit.vert.glsl", "Unlit.frag.glsl"));
#ifdef DISTRIBUTION
		ShaderLibrary::Add("ShadowMap", Shader::Create("ShadowMap.vert.glsl", "ShadowMap.frag.glsl"));
#else
		ShaderLibrary::Add("ShadowMap", Shader::Create("ShadowMap.vert.glsl", "ShadowMapRender.frag.glsl"));
#endif
		ShaderLibrary::Add("FullscreenQuad", Shader::Create("FullscreenQuad.vert.glsl", "FullscreenQuad.frag.glsl"));
		ShaderLibrary::Add("Tex2D", Shader::Create("Tex2D.vert.glsl", "Tex2D.frag.glsl"));
		ShaderLibrary::Add("SkyboxProc", Shader::Create("Skybox\\Procedural.vert.glsl", "Skybox\\Procedural.frag.glsl"));
		ShaderLibrary::Add("SkyboxPano", Shader::Create("Skybox\\Panoramic.vert.glsl", "Skybox\\Panoramic.frag.glsl"));
		ShaderLibrary::Add("Highlight", Shader::Create("Highlight.vert.glsl", "Highlight.frag.glsl"));
		ShaderLibrary::Add("Fog", Shader::Create("Fog.vert.glsl", "Fog.frag.glsl"));
		ShaderLibrary::Add("Particle", Shader::Create("Particle\\Particle.geom.glsl", "Particle\\Particle.vert.glsl", "Particle\\Particle.frag.glsl"));
		
		////compute shaders
		ShaderLibrary::Add("EmitterStep", Shader::Create("Particle\\EmitterStep.glsl"));
		ShaderLibrary::Add("Emitter", Shader::Create("Particle\\Emitter.glsl"));
		ShaderLibrary::Add("ParticleStep", Shader::Create("Particle\\ParticleStep.glsl"));
	}

	void Renderer::InitGeomPass() {
		//Init framebuffer
		Graphics::FramebufferSpec framebufferSpec;
		framebufferSpec.width = WINDOW_WIDTH<int>;
		framebufferSpec.height = WINDOW_HEIGHT<int>;
		framebufferSpec.attachments = { Graphics::FramebufferTextureFormat::RGBA8, Graphics::FramebufferTextureFormat::RED_INTEGER, Graphics::FramebufferTextureFormat::RGBA32F, Graphics::FramebufferTextureFormat::DEPTH };
		std::shared_ptr<Framebuffer> fb = Framebuffer::Create(framebufferSpec);
		InitSkyboxPass(fb);

		PipelineSpec geomPipelineSpec;
		geomPipelineSpec.shader = ShaderLibrary::Get("PBR");
		geomPipelineSpec.targetFramebuffer = fb;
		geomPipelineSpec.lineWidth = 2.5f;

		RenderPassSpec geomPassSpec;
		geomPassSpec.pipeline = Pipeline::Create(geomPipelineSpec);
		geomPassSpec.debugName = "Geometry Pass";


		AddPass(RenderPass::Create<GeomPass>(geomPassSpec));
	}

	void Renderer::InitPickPass() {
		/*Graphics::FramebufferSpec pickBufferSpec;
		pickBufferSpec.width = WINDOW_WIDTH<int>;
		pickBufferSpec.height = WINDOW_HEIGHT<int>;
		pickBufferSpec.attachments = { Graphics::FramebufferTextureFormat::RED_INTEGER, Graphics::FramebufferTextureFormat::DEPTH };

		PipelineSpec pickPipelineSpec;
		pickPipelineSpec.shader = ShaderLibrary::Get("PBR");
		pickPipelineSpec.targetFramebuffer = Framebuffer::Create(pickBufferSpec);

		RenderPassSpec pickPassSpec;
		pickPassSpec.pipeline = Pipeline::Create(pickPipelineSpec);
		pickPassSpec.debugName = "Picking Pass";

		AddPass(RenderPass::Create<PickPass>(pickPassSpec));*/
	}

	void Renderer::InitSkyboxPass(std::shared_ptr<Framebuffer> const& fb){
		PipelineSpec skyboxPSpec;
		skyboxPSpec.shader = ShaderLibrary::Get("SkyboxProc");
		//skyboxPSpec.shader = ShaderLibrary::Get("SkyboxPano");
		skyboxPSpec.targetFramebuffer = fb;

		RenderPassSpec skyboxPassSpec;
		skyboxPassSpec.pipeline = Pipeline::Create(skyboxPSpec);
		skyboxPassSpec.debugName = "Skybox Pass";

		AddPass(RenderPass::Create<SkyboxPass>(skyboxPassSpec));
	}

	void Renderer::InitShadowMapPass() {
		Graphics::FramebufferSpec shadowSpec;
		shadowSpec.width = shadowSpec.height = 2048;

		// @TODO: Allow for multiple shadow maps; need extend code
		//				to use glTexImage3D and GL_TEXTURE_2D_ARRAY
		shadowSpec.attachments = { Graphics::FramebufferTextureFormat::SHADOW_MAP };	// temporarily max. 1 shadow-caster
#ifndef DISTRIBUTION
		// only add color buffer for engine use
		shadowSpec.attachments.attachments.emplace_back(Graphics::FramebufferTextureFormat::RGBA8);
#endif

		PipelineSpec shadowPSpec;
		shadowPSpec.shader = ShaderLibrary::Get("ShadowMap");
		shadowPSpec.targetFramebuffer = Framebuffer::Create(shadowSpec);

		RenderPassSpec shadowPassSpec;
		shadowPassSpec.pipeline = Pipeline::Create(shadowPSpec);

		shadowPassSpec.debugName = "Shadow Map Pass";

		AddPass(RenderPass::Create<ShadowPass>(shadowPassSpec));
	}

	void Renderer::InitScreenPass(std::shared_ptr<Framebuffer> const& fb) { //might remove
		//Graphics::FramebufferSpec screenSpec;
		//screenSpec.width = WINDOW_WIDTH<int>;
		//screenSpec.height = WINDOW_HEIGHT<int>;
		//screenSpec.attachments = { Graphics::FramebufferTextureFormat::RGBA8 };

		PipelineSpec screenPSpec;
		screenPSpec.shader = ShaderLibrary::Get("FullscreenQuad");
		screenPSpec.targetFramebuffer = fb;

		RenderPassSpec screenPassSpec;
		screenPassSpec.pipeline = Pipeline::Create(screenPSpec);
		screenPassSpec.debugName = "Screen Pass";

		AddPass(RenderPass::Create<ScreenPass>(screenPassSpec));
	}

	void Renderer::InitPostProcessPass()
	{
		Graphics::FramebufferSpec postprocessSpec;
		postprocessSpec.width = WINDOW_WIDTH<int>;
		postprocessSpec.height = WINDOW_HEIGHT<int>;
		postprocessSpec.attachments = { Graphics::FramebufferTextureFormat::RGBA8 };	// temporarily max. 1 shadow-caster

		PipelineSpec postprocessPSpec;

		//leaving this blank, i have multipel shaders in postproc mgr to be run in a pingpong fashion
		postprocessPSpec.shader = nullptr; //ShaderLibrary::Get("ShadowMap");
		postprocessPSpec.targetFramebuffer = Framebuffer::Create(postprocessSpec);

		RenderPassSpec postprocessPassSpec;
		postprocessPassSpec.pipeline = Pipeline::Create(postprocessPSpec);
		postprocessPassSpec.debugName = "Post Process Pass";

		AddPass(RenderPass::Create<PostProcessingPass>(postprocessPassSpec));
	}

	void Renderer::InitParticlePass()
	{
		Graphics::FramebufferSpec particleSpec;
		particleSpec.width = WINDOW_WIDTH<int>;
		particleSpec.height = WINDOW_HEIGHT<int>;
		particleSpec.attachments = { Graphics::FramebufferTextureFormat::RGBA8 };	// temporarily max. 1 shadow-caster

		PipelineSpec particlePSpec;

		//leaving this blank, i have multipel shaders in postproc mgr to be run in a pingpong fashion
		particlePSpec.shader = ShaderLibrary::Get("Particle");
		particlePSpec.targetFramebuffer = Framebuffer::Create(particleSpec);

		RenderPassSpec particlePassSpec;
		particlePassSpec.pipeline = Pipeline::Create(particlePSpec);
		particlePassSpec.debugName = "Particle Pass";

		AddPass(RenderPass::Create<ParticlePass>(particlePassSpec));
	}

	void Renderer::InitUIPass() {
		Graphics::FramebufferSpec fbSpec;
		fbSpec.width = WINDOW_WIDTH<int>;
		fbSpec.height = WINDOW_HEIGHT<int>;
		fbSpec.attachments = { Graphics::FramebufferTextureFormat::RGBA8 };
		auto const& fb = Framebuffer::Create(fbSpec);

		InitScreenPass(fb);

		PipelineSpec uiPSpec;
		uiPSpec.shader = ShaderLibrary::Get("Tex2D");
		uiPSpec.targetFramebuffer = fb;
		uiPSpec.lineWidth = 2.5f;

		RenderPassSpec uiPassSpec;
		uiPassSpec.pipeline = Pipeline::Create(uiPSpec);
		uiPassSpec.debugName = "UI Pass";

		AddPass(RenderPass::Create<UIPass>(uiPassSpec));
	}

	void Renderer::InitMeshSources(){
		//mData.debugMeshSources[0] = IGE_ASSETMGR.LoadRef<IGE::Assets::ModelAsset>("Cube");
		mData.quadMeshSource = { IGE_ASSETMGR.LoadRef<IGE::Assets::ModelAsset>("Quad") };
	}

	void Renderer::InitFullscreenQuad(){
		//Setting up Fullscreen Quad
		mData.screen.screenVertices[0] = { {-1.0f,  1.0f}, {0.0f, 1.0f} };
		mData.screen.screenVertices[1] = { {-1.0f, -1.0f}, {0.0f, 0.0f} };
		mData.screen.screenVertices[2] = { { 1.0f, -1.0f}, {1.0f, 0.0f} };	
		mData.screen.screenVertices[3] = { {-1.0f,  1.0f}, {0.0f, 1.0f} };
		mData.screen.screenVertices[4] = { { 1.0f, -1.0f}, {1.0f, 0.0f} };
		mData.screen.screenVertices[5] = { { 1.0f,  1.0f}, {1.0f, 1.0f} };

		mData.screen.screenVertexArray = VertexArray::Create();
		mData.screen.screenVertexBuffer = VertexBuffer::Create(sizeof(mData.screen.screenVertices));

		mData.screen.screenVertexBuffer->Bind();
		mData.screen.screenVertexBuffer->SetData(mData.screen.screenVertices.data(), sizeof(mData.screen.screenVertices));
		BufferLayout screenLayout = {
			{AttributeType::VEC2, "a_Position"},
			{AttributeType::VEC2, "a_TexCoord"},
		};
		mData.screen.screenVertexBuffer->SetLayout(screenLayout);
		mData.screen.screenVertexArray->AddVertexBuffer(mData.screen.screenVertexBuffer);
	}

	void Renderer::InitUICamera(){
		// Initialize the UI cam with an orthographic projection
		mUICamera.projType = Component::Camera::Type::ORTHO;
		mUICamera.position = glm::vec3(0.0f, 0.0f, 0.0f);  // Centered for screen space
		mUICamera.aspectRatio = 16.0f / 9.0f;              // Adjust based on screen dimensions
		mUICamera.nearClip = -100.0f;
		mUICamera.farClip = 100.0f;
		mUICamera.left =  - mUICamera.aspectRatio * UI_SCALING_FACTOR<float>;
		mUICamera.right = mUICamera.aspectRatio * UI_SCALING_FACTOR<float>;
		mUICamera.bottom = -10.0f;
		mUICamera.top = 10.0f;
	}

	void Renderer::HandleUIInput(std::vector<ECS::Entity> const& entities) {
		static ECS::Entity prevHoveredUIEntity{};

		glm::vec2 mousePos = Input::InputManager::GetInstance().GetMousePos();

		// Step 1: Perform UI picking
		ECS::Entity const hoveredUIEntity = Renderer::PickUIEntity(mousePos, entities);

		// Step 2: Handle hover state changes
		if (hoveredUIEntity != prevHoveredUIEntity) {
			if (prevHoveredUIEntity) {
				// Trigger Pointer Exit event for the previously hovered UI element
				QUEUE_EVENT(Events::EntityPointerExit, prevHoveredUIEntity);
			}
			if (hoveredUIEntity) {
				// Trigger Pointer Enter event for the newly hovered UI element
				QUEUE_EVENT(Events::EntityPointerEnter, hoveredUIEntity);
			}
		}

		// Step 3: Track hovered UI entity
		prevHoveredUIEntity = hoveredUIEntity;

		// Step 4: Handle pointer press/release states
		if (hoveredUIEntity) {
			if (Input::InputManager::GetInstance().IsKeyTriggered(IK_MOUSE_LEFT)) {
				// Trigger Pointer Down event
				QUEUE_EVENT(Events::EntityPointerDown, hoveredUIEntity);
			}
			if (Input::InputManager::GetInstance().IsKeyReleased(IK_MOUSE_LEFT)) {
				// Trigger Pointer Up event
				QUEUE_EVENT(Events::EntityPointerUp, hoveredUIEntity);
			}
		}
	}

	glm::vec2 Renderer::ConvertMouseToCanvasSpace(glm::vec2 const& mousePos, glm::vec4 const& orthoBounds, glm::vec2 const& screenSize) {
		glm::vec2 normalizedMousePos = mousePos / screenSize; // Normalize to [0, 1]


		glm::vec2 canvasPos;
		canvasPos.x = orthoBounds.x + normalizedMousePos.x * (orthoBounds.y - orthoBounds.x); // Map to canvas X
		canvasPos.y = orthoBounds.z +(1.f - normalizedMousePos.y) * (orthoBounds.w - orthoBounds.z); // Map to canvas Y
		return canvasPos;
	}

	void Renderer::SetHighlightedEntity(ECS::Entity const& entity) {
		mHighlightedEntity = entity;
	}

	ECS::Entity Renderer::GetHighlightedEntity(){
		return mHighlightedEntity;
	}

	EVENT_CALLBACK_DEF(Renderer, OnResize) {
		auto const& e { CAST_TO_EVENT(Events::WindowResized)};

		for (auto const& pass : mRenderPasses) {
			pass->GetTargetFramebuffer()->Resize(e->mWidth, e->mHeight);
		}
	}

	EVENT_CALLBACK_DEF(Renderer, OnEntityPicked) {
		ECS::Entity const& entity { CAST_TO_EVENT(Events::EntityScreenPicked)->mEntity };
		SetHighlightedEntity(entity);
	}

	void Renderer::Shutdown() {
		MaterialTable::Shutdown();
	}

	void Renderer::Clear(){
		RenderAPI::Clear();
	}

	void Renderer::SetQuadBufferData(glm::vec3 const& pos, glm::vec4 const& clr, glm::vec2 const& texCoord, float texIdx, int entity) {
		UNREFERENCED_PARAMETER(entity);
		if (mData.quadBufferIndex < mData.quadBuffer.size()) {
			QuadVtx& vtx = mData.quadBuffer[mData.quadBufferIndex];
			vtx.pos = pos;
			vtx.texCoord = texCoord;
			vtx.texIdx = texIdx;
			vtx.clr = clr;
			vtx.entityID = entity;
		}
		++mData.quadBufferIndex;
	}

	void Renderer::SetTriangleBufferData(glm::vec3 const& pos, glm::vec4 const& clr) {
		if (mData.triVtxCount < mData.triBuffer.size()) {
			LineVtx& vtx = mData.triBuffer[mData.triBufferIndex];
			vtx.pos = pos;
			vtx.clr = clr;
		}
		++mData.triBufferIndex;
		++mData.triVtxCount;
	}
	
	void Renderer::SetLineBufferData(glm::vec3 const& pos, glm::vec4 const& clr) {
		if (mData.lineBufferIndex >= mData.lineBuffer.size()) {
#ifdef _DEBUG
			std::cerr << "Error: Line buffer index out of range! Index: " << mData.lineBufferIndex << ", Max: " << mData.lineBuffer.size() << std::endl;
#endif
			return; // Prevent writing out of bounds
		}
		if (mData.lineVtxCount < mData.lineBuffer.size()) {
			LineVtx& vtx = mData.lineBuffer[mData.lineBufferIndex];
			vtx.pos = pos;
			vtx.clr = clr;
		}
		++mData.lineBufferIndex;
		++mData.lineVtxCount;
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
		auto instanceBuffer = VertexBuffer::Create(cInstanceCap * sizeof(InstanceData));

		// Set up the buffer layout for instance data
		BufferLayout instanceLayout = {
			{ AttributeType::MAT4, "a_ModelMatrix" },
			{ AttributeType::INT, "a_MaterialIdx"},
			{ AttributeType::INT, "a_EntityID"}
			//{ AttributeType::VEC4, "a_Color" },

		};

		instanceBuffer->SetLayout(instanceLayout);

		// Attach the instance buffer to the MeshSource's VAO
		IGE_REF(IGE::Assets::ModelAsset, meshSrc)->mMeshSource.GetVertexArray()->AddVertexBuffer(instanceBuffer, true);

		// Store the buffer in the map for future use
		mData.instanceBuffers[meshSrc] = instanceBuffer;

		// Return the new instance buffer
		return instanceBuffer;
	}

	Renderer::~Renderer(){
		Renderer::Shutdown();
	}

	void Renderer::DrawLine(glm::vec3 const& p0, glm::vec3 const& p1, glm::vec4 const& clr) {
		if (mData.lineVtxCount >= RendererData::cMaxVertices2D)
			NextBatch();

		SetLineBufferData(p0, clr);
		SetLineBufferData(p1, clr);

		++mData.stats.lineCount;
	}

	void Renderer::DrawRect(glm::vec3 const& pos, glm::vec2 const& scale, glm::quat const& rot, glm::vec4 const& clr) {
		glm::mat4 translateMtx{ glm::translate(glm::mat4{ 1.f }, pos) };
		glm::mat4 rotateMtx{ glm::toMat4(rot) };
		glm::mat4 scaleMtx{ glm::scale(glm::mat4{ 1.f }, { scale.x, scale.y, 1.f }) };
		glm::mat4 transformMtx{ translateMtx * rotateMtx * scaleMtx };

		glm::vec3 lineVertices[4]{};
		for (size_t i{}; i < 4; ++i)
			lineVertices[i] = transformMtx * mData.quadVtxPos[i];

		DrawLine(lineVertices[0], lineVertices[1], clr);
		DrawLine(lineVertices[1], lineVertices[2], clr);
		DrawLine(lineVertices[2], lineVertices[3], clr);
		DrawLine(lineVertices[3], lineVertices[0], clr);

	}

	void Renderer::DrawQuad(glm::vec3 const& pos, glm::vec2 const& scale, glm::quat const& rot, glm::vec4 const& clr, int entity) {
		if (mData.quadIdxCount >= RendererData::cMaxIndices2D)
			NextBatch();

		constexpr glm::vec2 texCoords[4]{ { 0.f, 0.f }, {1.f, 0.f}, {1.f, 1.f}, {0.f, 1.f} };
		const float texIdx{}; // white tex index = 0

		glm::mat4 translateMtx{ glm::translate(glm::mat4{ 1.f }, pos) };
		glm::mat4 rotateMtx{ glm::toMat4(rot)};
		glm::mat4 scaleMtx{ glm::scale(glm::mat4{ 1.f }, { scale.x, scale.y, 1.f }) };
		glm::mat4 transformMtx{ translateMtx * rotateMtx * scaleMtx };

		for (size_t i{}; i < 4; ++i)
			SetQuadBufferData(transformMtx * mData.quadVtxPos[i], clr, texCoords[i], texIdx, entity);

		mData.quadIdxCount += 6;
		++mData.stats.quadCount;
	}

	void Renderer::DrawSprite(glm::vec3 const& pos, glm::vec2 const& scale, glm::quat const& rot, Texture const& tex, glm::vec4 const& tint, int entity, bool isBillboard, CameraSpec const& cam){

		if (tex.GetTexHdl() == 0)
			DrawQuad(pos, scale, rot, tint);
		if (mData.quadIdxCount >= RendererData::cMaxIndices2D)
			NextBatch();

		//std::array<glm::vec2, 4> texCoords{ };
		//std::shared_ptr<Texture> tex = subtex->GetTexture();
		//if (subtex->GetProperties().id == 1698985226353418500) {
		//	int i = 1;
		//	UNREFERENCED_PARAMETER(i);
		//}
		constexpr glm::vec2 texCoords[4]{ { 0.f, 0.f }, {1.f, 0.f}, {1.f, 1.f}, {0.f, 1.f} };
		float texIdx = 0.f;

		//don't need to iterate all 32 slots every time
		for (uint32_t i{ 1 }; i < mData.texUnitIdx; ++i) {
			if (mData.texUnits[i] == tex) { //check if the particular texture has already been set
				texIdx = static_cast<float>(i);
				break;
			}
		}

		if (texIdx == 0.f) {
			texIdx = static_cast<float>(mData.texUnitIdx);
			mData.texUnits[mData.texUnitIdx] = tex;
			++mData.texUnitIdx;
		}

		glm::mat4 translateMtx{ glm::translate(glm::mat4{ 1.f }, pos) };
		glm::mat4 rotateMtx{ glm::toMat4(rot) };
		glm::mat4 scaleMtx{ glm::scale(glm::mat4{ 1.f }, { scale.x, scale.y, 1.f }) };

		if (isBillboard) {
			rotateMtx = glm::mat4{ 1.f };
			glm::mat4 viewMatrix = cam.viewMatrix; // Use the correct camera
			glm::vec3 cameraRight{ viewMatrix[0][0], viewMatrix[1][0], viewMatrix[2][0] };
			glm::vec3 cameraUp{ viewMatrix[0][1], viewMatrix[1][1], viewMatrix[2][1] };

			rotateMtx[0] = glm::vec4(cameraRight, 0.0f);
			rotateMtx[1] = glm::vec4(cameraUp, 0.0f);
		}
		glm::mat4 transformMtx{ translateMtx * rotateMtx * scaleMtx };

		for (size_t i{}; i < 4; ++i)
			SetQuadBufferData(transformMtx * mData.quadVtxPos[i], tint, texCoords[i], texIdx, entity);

		mData.quadIdxCount += 6;
		++mData.stats.quadCount;
	}

	void Renderer::DrawBox(glm::vec3 const& pos, glm::vec3 const& scale, glm::quat const& rot, glm::vec4 const& clr){
		// Define the 8 vertices of a unit cube centered at the origin
		glm::vec3 cubeVertices[8] = {
			{ -0.5f, -0.5f, -0.5f }, // 0: Bottom-left-back
			{  0.5f, -0.5f, -0.5f }, // 1: Bottom-right-back
			{  0.5f,  0.5f, -0.5f }, // 2: Top-right-back
			{ -0.5f,  0.5f, -0.5f }, // 3: Top-left-back
			{ -0.5f, -0.5f,  0.5f }, // 4: Bottom-left-front
			{  0.5f, -0.5f,  0.5f }, // 5: Bottom-right-front
			{  0.5f,  0.5f,  0.5f }, // 6: Top-right-front
			{ -0.5f,  0.5f,  0.5f }  // 7: Top-left-front
		};

		// Build the transformation matrix
		glm::mat4 translateMtx = glm::translate(glm::mat4(1.0f), pos);
		glm::mat4 rotateMtx = glm::toMat4(rot);
		glm::mat4 scaleMtx = glm::scale(glm::mat4(1.0f), scale);
		glm::mat4 transformMtx = translateMtx * rotateMtx * scaleMtx;

		// Transform each vertex
		glm::vec3 transformedVertices[8];
		for (int i = 0; i < 8; ++i) {
			transformedVertices[i] = transformMtx * glm::vec4(cubeVertices[i], 1.0f);
		}

		// Define the 12 edges of the cube by connecting vertex pairs
		int edges[12][2] = {
			{ 0, 1 }, { 1, 2 }, { 2, 3 }, { 3, 0 }, // Back face
			{ 4, 5 }, { 5, 6 }, { 6, 7 }, { 7, 4 }, // Front face
			{ 0, 4 }, { 1, 5 }, { 2, 6 }, { 3, 7 }  // Connecting edges between front and back faces
		};

		// Draw each edge as a line
		for (int i = 0; i < 12; ++i) {
			DrawLine(transformedVertices[edges[i][0]], transformedVertices[edges[i][1]], clr);
		}
	}

	void Renderer::DrawBox(glm::mat4 const& mtx, glm::vec4 const& clr){
		// Define the 8 vertices of a unit cube centered at the origin
		glm::vec3 cubeVertices[8] = {
			{ -0.5f, -0.5f, -0.5f }, // 0: Bottom-left-back
			{  0.5f, -0.5f, -0.5f }, // 1: Bottom-right-back
			{  0.5f,  0.5f, -0.5f }, // 2: Top-right-back
			{ -0.5f,  0.5f, -0.5f }, // 3: Top-left-back
			{ -0.5f, -0.5f,  0.5f }, // 4: Bottom-left-front
			{  0.5f, -0.5f,  0.5f }, // 5: Bottom-right-front
			{  0.5f,  0.5f,  0.5f }, // 6: Top-right-front
			{ -0.5f,  0.5f,  0.5f }  // 7: Top-left-front
		};


		// Transform each vertex
		glm::vec3 transformedVertices[8];
		for (int i = 0; i < 8; ++i) {
			transformedVertices[i] = mtx * glm::vec4(cubeVertices[i], 1.0f);
		}

		// Define the 12 edges of the cube by connecting vertex pairs
		int edges[12][2] = {
			{ 0, 1 }, { 1, 2 }, { 2, 3 }, { 3, 0 }, // Back face
			{ 4, 5 }, { 5, 6 }, { 6, 7 }, { 7, 4 }, // Front face
			{ 0, 4 }, { 1, 5 }, { 2, 6 }, { 3, 7 }  // Connecting edges between front and back faces
		};

		// Draw each edge as a line
		for (int i = 0; i < 12; ++i) {
			DrawLine(transformedVertices[edges[i][0]], transformedVertices[edges[i][1]], clr);
		}
	}

	void Renderer::DrawWireSphere(glm::vec3 const& pos, float radius, glm::vec4 const& clr, int numCircles) {
		const int segments = 32; // Number of segments to approximate the circle
		const float deltaAngle = glm::two_pi<float>() / segments;
		glm::mat4 translateMtx = glm::translate(glm::mat4(1.0f), pos);
		glm::mat4 scaleMtx = glm::scale(glm::mat4(1.0f), glm::vec3(radius));
		glm::mat4 transformMtx = translateMtx * scaleMtx;

		// Draw base circles in the XY, XZ, and YZ planes
		auto drawCircle = [&](glm::vec3 axis1, glm::vec3 axis2) {
			for (int i = 0; i < segments; ++i) {
				float angle1 = i * deltaAngle;
				float angle2 = (i + 1) * deltaAngle;

				glm::vec3 p1 = glm::vec3(axis1 * cos(angle1) + axis2 * sin(angle1));
				glm::vec3 p2 = glm::vec3(axis1 * cos(angle2) + axis2 * sin(angle2));

				glm::vec3 transformedP1 = transformMtx * glm::vec4(p1, 1.0f);
				glm::vec3 transformedP2 = transformMtx * glm::vec4(p2, 1.0f);

				DrawLine(transformedP1, transformedP2, clr);
			}
		};

		// Base circles
		drawCircle(glm::vec3(1, 0, 0), glm::vec3(0, 1, 0)); // XY plane
		drawCircle(glm::vec3(1, 0, 0), glm::vec3(0, 0, 1)); // XZ plane
		drawCircle(glm::vec3(0, 1, 0), glm::vec3(0, 0, 1)); // YZ plane

		// Draw circles rotated around the X-axis
		for (int i = 0; i < numCircles; ++i) {
			float angle = glm::radians(360.0f * i / numCircles);
			glm::mat4 rotX = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(1.0f, 0.0f, 0.0f));
			glm::mat4 rotatedTransform = transformMtx * rotX;

			for (int j = 0; j < segments; ++j) {
				float angle1 = j * deltaAngle;
				float angle2 = (j + 1) * deltaAngle;

				glm::vec3 p1 = glm::vec3(cos(angle1), 0.0f, sin(angle1));
				glm::vec3 p2 = glm::vec3(cos(angle2), 0.0f, sin(angle2));

				glm::vec3 transformedP1 = rotatedTransform * glm::vec4(p1, 1.0f);
				glm::vec3 transformedP2 = rotatedTransform * glm::vec4(p2, 1.0f);

				DrawLine(transformedP1, transformedP2, clr);
			}
		}

		// Draw circles rotated around the Y-axis
		for (int i = 0; i < numCircles; ++i) {
			float angle = glm::radians(360.0f * i / numCircles);
			glm::mat4 rotY = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 1.0f, 0.0f));
			glm::mat4 rotatedTransform = transformMtx * rotY;

			for (int j = 0; j < segments; ++j) {
				float angle1 = j * deltaAngle;
				float angle2 = (j + 1) * deltaAngle;

				glm::vec3 p1 = glm::vec3(0.0f, cos(angle1), sin(angle1));
				glm::vec3 p2 = glm::vec3(0.0f, cos(angle2), sin(angle2));

				glm::vec3 transformedP1 = rotatedTransform * glm::vec4(p1, 1.0f);
				glm::vec3 transformedP2 = rotatedTransform * glm::vec4(p2, 1.0f);

				DrawLine(transformedP1, transformedP2, clr);
			}
		}
	}


	void Renderer::DrawWireCapsule(glm::mat4 const& transformMtx, float radius, float height, glm::vec4 const& clr, int numLines) {
		const int segments = 16; // Number of segments for circular edges
		const float deltaAngle = glm::two_pi<float>() / segments;
		const float halfHeight = (height - 2.0f * radius) / 2.0f; // Half the cylindrical height
		glm::mat4 radiusScale = glm::scale(glm::mat4(1.0f), glm::vec3(radius));

		// Draw top and bottom circles of the cylinder
		for (int i = 0; i < segments; ++i) {
			float angle1 = i * deltaAngle;
			float angle2 = (i + 1) * deltaAngle;

			glm::vec3 p1Bottom = glm::vec3(cos(angle1), -halfHeight, sin(angle1));
			glm::vec3 p2Bottom = glm::vec3(cos(angle2), -halfHeight, sin(angle2));

			glm::vec3 p1Top = glm::vec3(cos(angle1), halfHeight, sin(angle1));
			glm::vec3 p2Top = glm::vec3(cos(angle2), halfHeight, sin(angle2));

			glm::vec3 transformedP1Bottom = transformMtx * radiusScale * glm::vec4(p1Bottom, 1.0f);
			glm::vec3 transformedP2Bottom = transformMtx * radiusScale * glm::vec4(p2Bottom, 1.0f);

			glm::vec3 transformedP1Top = transformMtx * radiusScale * glm::vec4(p1Top, 1.0f);
			glm::vec3 transformedP2Top = transformMtx * radiusScale * glm::vec4(p2Top, 1.0f);

			// Draw the circles at the top and bottom
			DrawLine(transformedP1Bottom, transformedP2Bottom, clr);
			DrawLine(transformedP1Top, transformedP2Top, clr);
		}

		// Draw vertical lines connecting the top and bottom hemispheres
		for (int i = 0; i < segments; ++i) {
			float angle = i * deltaAngle;

			glm::vec3 pBottom = glm::vec3(cos(angle), -halfHeight, sin(angle));
			glm::vec3 pTop = glm::vec3(cos(angle), halfHeight, sin(angle));

			glm::vec3 transformedPBottom = transformMtx * radiusScale * glm::vec4(pBottom, 1.0f);
			glm::vec3 transformedPTop = transformMtx * radiusScale * glm::vec4(pTop, 1.0f);

			DrawLine(transformedPBottom, transformedPTop, clr);
		}

		// Draw longitudinal arcs (curved vertical lines) around the capsule
		for (int i = 0; i < numLines; ++i) {
			float angle = i * glm::two_pi<float>() / numLines;

			for (int j = 0; j <= segments / 4; ++j) { // Split hemisphere into 4 vertical sections
				float theta1 = j * glm::half_pi<float>() / (segments / 4);
				float theta2 = (j + 1) * glm::half_pi<float>() / (segments / 4);

				// Top hemisphere arcs
				glm::vec3 p1Top = glm::vec3(cos(angle) * sin(theta1), halfHeight + radius * cos(theta1), sin(angle) * sin(theta1));
				glm::vec3 p2Top = glm::vec3(cos(angle) * sin(theta2), halfHeight + radius * cos(theta2), sin(angle) * sin(theta2));

				// Bottom hemisphere arcs
				glm::vec3 p1Bottom = glm::vec3(cos(angle) * sin(theta1), -halfHeight - radius * cos(theta1), sin(angle) * sin(theta1));
				glm::vec3 p2Bottom = glm::vec3(cos(angle) * sin(theta2), -halfHeight - radius * cos(theta2), sin(angle) * sin(theta2));

				glm::vec3 transformedP1Top = transformMtx * radiusScale * glm::vec4(p1Top, 1.0f);
				glm::vec3 transformedP2Top = transformMtx * radiusScale * glm::vec4(p2Top, 1.0f);

				glm::vec3 transformedP1Bottom = transformMtx * radiusScale * glm::vec4(p1Bottom, 1.0f);
				glm::vec3 transformedP2Bottom = transformMtx * radiusScale * glm::vec4(p2Bottom, 1.0f);

				// Draw arcs for top and bottom hemispheres
				DrawLine(transformedP1Top, transformedP2Top, clr);
				DrawLine(transformedP1Bottom, transformedP2Bottom, clr);
			}
		}
	}

	void Renderer::DrawCameraFrustrum(Component::Camera const& cam, glm::vec4 const& clr){
		float aspectRatio = cam.aspectRatio;
		float tanHalfFOV = tan(glm::radians(cam.fov) / 2.0f);
		float nearHeight = 2.0f * tanHalfFOV * cam.nearClip;
		float nearWidth = nearHeight * aspectRatio;
		float farHeight = 2.0f * tanHalfFOV * cam.farClip;
		float farWidth = farHeight * aspectRatio;
		
		// Define frustum corners in view space
		glm::vec3 nearCenter = glm::vec3(0.0f, 0.0f, -cam.nearClip);
		glm::vec3 farCenter = glm::vec3(0.0f, 0.0f, -cam.farClip);
		
		glm::vec3 nearTopLeft = nearCenter + glm::vec3(-nearWidth / 2.0f, nearHeight / 2.0f, 0.0f);
		glm::vec3 nearTopRight = nearCenter + glm::vec3(nearWidth / 2.0f, nearHeight / 2.0f, 0.0f);
		glm::vec3 nearBottomLeft = nearCenter + glm::vec3(-nearWidth / 2.0f, -nearHeight / 2.0f, 0.0f);
		glm::vec3 nearBottomRight = nearCenter + glm::vec3(nearWidth / 2.0f, -nearHeight / 2.0f, 0.0f);
		
		glm::vec3 farTopLeft = farCenter + glm::vec3(-farWidth / 2.0f, farHeight / 2.0f, 0.0f);
		glm::vec3 farTopRight = farCenter + glm::vec3(farWidth / 2.0f, farHeight / 2.0f, 0.0f);
		glm::vec3 farBottomLeft = farCenter + glm::vec3(-farWidth / 2.0f, -farHeight / 2.0f, 0.0f);
		glm::vec3 farBottomRight = farCenter + glm::vec3(farWidth / 2.0f, -farHeight / 2.0f, 0.0f);
		
		// Transform frustum corners to world space
		glm::mat4 viewMatrix = cam.GetViewMatrix();
		glm::mat4 inverseViewMatrix = glm::inverse(viewMatrix);
		
		nearTopLeft = glm::vec3(inverseViewMatrix * glm::vec4(nearTopLeft, 1.0f));
		nearTopRight = glm::vec3(inverseViewMatrix * glm::vec4(nearTopRight, 1.0f));
		nearBottomLeft = glm::vec3(inverseViewMatrix * glm::vec4(nearBottomLeft, 1.0f));
		nearBottomRight = glm::vec3(inverseViewMatrix * glm::vec4(nearBottomRight, 1.0f));
		
		farTopLeft = glm::vec3(inverseViewMatrix * glm::vec4(farTopLeft, 1.0f));
		farTopRight = glm::vec3(inverseViewMatrix * glm::vec4(farTopRight, 1.0f));
		farBottomLeft = glm::vec3(inverseViewMatrix * glm::vec4(farBottomLeft, 1.0f));
		farBottomRight = glm::vec3(inverseViewMatrix * glm::vec4(farBottomRight, 1.0f));
		
		// Draw frustum edges
		// Near plane
		DrawLine(nearTopLeft, nearTopRight, clr);
		DrawLine(nearTopRight, nearBottomRight, clr);
		DrawLine(nearBottomRight, nearBottomLeft, clr);
		DrawLine(nearBottomLeft, nearTopLeft, clr);
		
		// Far plane
		DrawLine(farTopLeft, farTopRight, clr);
		DrawLine(farTopRight, farBottomRight, clr);
		DrawLine(farBottomRight, farBottomLeft, clr);
		DrawLine(farBottomLeft, farTopLeft, clr);
		
		// Connecting edges
		DrawLine(nearTopLeft, farTopLeft, clr);
		DrawLine(nearTopRight, farTopRight, clr);
		DrawLine(nearBottomLeft, farBottomLeft, clr);
		DrawLine(nearBottomRight, farBottomRight, clr);
	}

	void Renderer::DrawArrow(glm::vec3 const& start, glm::vec3 const& end, glm::vec4 const& clr){
		// Calculate direction vector
		glm::vec3 direction = glm::normalize(end - start);

		// Arrow length
		float arrowLength = glm::length(end - start);

		// Scale for the arrowhead
		float headLength = arrowLength * 0.2f; // 20% of the arrow length
		float headWidth = headLength * 0.5f;

		// Calculate base of the arrowhead
		glm::vec3 arrowBase = end - direction * headLength;

		// Orthogonal vectors for arrowhead (perpendicular to direction)
		glm::vec3 perp1 = glm::normalize(glm::cross(direction, { 0.f, 1.f, 0.f })) * headWidth;
		glm::vec3 perp2 = glm::normalize(glm::cross(direction, perp1)) * headWidth;

		// Arrowhead points
		glm::vec3 left = arrowBase + perp1;
		glm::vec3 right = arrowBase - perp1;

		// Draw the arrow shaft
		DrawLine(start, end, clr);

		// Draw the arrowhead
		DrawLine(end, left, clr);
		DrawLine(end, right, clr);
		DrawLine(left, right, clr); // Optional: Connect the arrowhead base
	}

	void Renderer::DrawCone(glm::vec3 const& position, glm::vec3 const& direction, float range, float angle, glm::vec4 const& color) {
		// Normalize the direction to ensure it's a unit vector
		glm::vec3 worldDirection = glm::normalize(direction);

		// Calculate the radius of the base circle using the range and outer angle
		float radius = range * glm::tan(glm::radians(angle));

		// Determine the base center position
		glm::vec3 baseCenter = position + worldDirection * range;

		// Draw the base circle
		DrawCircle(baseCenter, radius, color * glm::vec4(1.f, 1.f, 1.f, 0.5f), worldDirection);

		// Calculate two edge points on the circle
		// For simplicity, use a fixed perpendicular vector to the direction
		glm::vec3 perpVec1 = glm::normalize(glm::cross(worldDirection, glm::vec3(0.f, 1.f, 0.f)));
		if (glm::length(perpVec1) < 0.001f) // Handle edge case where direction is parallel to (0, 1, 0)
			perpVec1 = glm::normalize(glm::cross(worldDirection, glm::vec3(1.f, 0.f, 0.f)));
		glm::vec3 perpVec2 = glm::normalize(glm::cross(worldDirection, perpVec1));

		// Edge points
		glm::vec3 edge1 = baseCenter + perpVec1 * radius;
		glm::vec3 edge2 = baseCenter - perpVec1 * radius;
		glm::vec3 edge3 = baseCenter + perpVec2 * radius;
		glm::vec3 edge4 = baseCenter - perpVec2 * radius;

		// Draw edges connecting the cone tip to the base circle
		DrawLine(position, edge1, color);
		DrawLine(position, edge2, color);
		DrawLine(position, edge3, color);
		DrawLine(position, edge4, color);
	}

	void Renderer::DrawCircle(glm::vec3 const& center, float radius, glm::vec4 const& color, glm::vec3 const& normal) {
		constexpr int segments = 32; // Number of segments for the circle
		std::vector<glm::vec3> points(segments);

		// Create a perpendicular vector to the normal
		glm::vec3 tangent = glm::normalize(glm::cross(normal, glm::vec3(0.f, 1.f, 0.f)));
		if (glm::length(tangent) < 0.001f) // Handle edge case where normal is parallel to (0, 1, 0)
			tangent = glm::normalize(glm::cross(normal, glm::vec3(1.f, 0.f, 0.f)));
		glm::vec3 bitangent = glm::normalize(glm::cross(normal, tangent));

		// Calculate points on the circle
		for (int i = 0; i < segments; ++i) {
			float theta = glm::radians(360.f * i / segments);
			glm::vec3 point = center + radius * (tangent * cos(theta) + bitangent * sin(theta));
			points[i] = point;
		}

		// Draw lines connecting the points
		for (int i = 0; i < segments; ++i) {
			int next = (i + 1) % segments; // Ensure the last point connects back to the first
			DrawLine(points[i], points[next], color);
		}
	}

	void Renderer::DrawLightGizmo(Component::Light const& light, Component::Transform const& xform, CameraSpec const& cam, int lightID){
		glm::vec3 worldDir = glm::normalize(xform.rotation * light.forwardVec);
		switch (light.type) {
			case Component::LightType::DIRECTIONAL:
				Renderer::DrawSprite(xform.worldPos, glm::vec2{ xform.worldScale }, xform.worldRot, IGE_ASSETMGR.GetAsset<IGE::Assets::TextureAsset>(mIcons[0])->mTexture, glm::vec4 { light.color, 1.f }, lightID, true, cam);
				glm::vec3 arrowTip = xform.worldPos + worldDir * 5.0f; // Scale the direction for visibility
				Renderer::DrawArrow(xform.worldPos, arrowTip, glm::vec4{ light.color, 1.f });
				Renderer::DrawWireSphere(xform.worldPos, light.mLightIntensity * 0.5f, glm::vec4{light.color, 1.f });
				break;
			case Component::LightType::SPOTLIGHT:
				Renderer::DrawSprite(xform.worldPos, glm::vec2{ xform.worldScale }, xform.worldRot, IGE_ASSETMGR.GetAsset<IGE::Assets::TextureAsset>(mIcons[1])->mTexture, glm::vec4 { light.color, 1.f }, lightID, true, cam);
				float angle = glm::radians(light.mOuterSpotAngle);

				// Draw spotlight cone
				DrawCone(xform.worldPos, worldDir, light.mRange, angle, glm::vec4(light.color, 0.5f));
				break;
		}
	}

	void Renderer::RenderFullscreenTexture(){
		glDisable(GL_BLEND);
		RenderAPI::DrawTriangles(mData.screen.screenVertexArray, 6);
		glEnable(GL_BLEND);
	}

	void Renderer::SubmitMesh(std::shared_ptr<Mesh> mesh, glm::vec3 const& pos, glm::vec3 const& rot, glm::vec3 const& scale, glm::vec4 const& clr) {
		if (mesh == nullptr) return;
		auto const& meshSrc = mesh->GetMeshSource();
		auto const& submeshes = IGE_REF(IGE::Assets::ModelAsset, meshSrc)->mMeshSource.GetSubmeshes();

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
				const Vertex& vtx = IGE_REF(IGE::Assets::ModelAsset, meshSrc)->mMeshSource.GetVertices()[submesh.baseVtx + i];

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

	void Renderer::SubmitInstance(IGE::Assets::GUID meshSource, glm::mat4 const& worldMtx, glm::vec4 const& clr, int id, int matID, int subID) {
		InstanceData instance{};
		instance.modelMatrix = worldMtx;
		//instance.color = clr;

		if (id != INVALID_ENTITY_ID) {
			instance.entityID = id;
		}
		instance.materialIdx = matID;
		
		SubmeshInstanceData subData;
		subData.submeshIdx = subID;
		subData.data = instance;
		mData.instanceBufferDataMap[meshSource].push_back(subData);
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
			
			auto& vao = IGE_REF(IGE::Assets::ModelAsset, meshSrc)->mMeshSource.GetVertexArray();

			RenderAPI::DrawIndicesInstanced(vao, static_cast<unsigned>(IGE_REF(IGE::Assets::ModelAsset, meshSrc)->mMeshSource.GetIndices().size()), static_cast<unsigned>(instances.size()));

		}

		mData.instanceBufferDataMap.clear();
	}

	void Renderer::RenderSubmeshInstances() {
		
		while (!mData.instanceBufferDataMap.empty()) {
			// collect the leftover entities after the batch
			decltype(mData.instanceBufferDataMap) overflow{};

			for (auto& [meshSrc, instances] : mData.instanceBufferDataMap) {
				if (instances.empty()) continue;
				// move any access into the overflow vector
				else if (instances.size() > cInstanceCap) {
					overflow.emplace(meshSrc,
						std::vector<SubmeshInstanceData>(
							std::make_move_iterator(instances.begin() + cInstanceCap),
							std::make_move_iterator(instances.end()))
					);

					instances.erase(instances.begin() + cInstanceCap, instances.end());
				}

				auto const& meshSource = IGE_REF(IGE::Assets::ModelAsset, meshSrc)->mMeshSource;
				auto const& submeshes = meshSource.GetSubmeshes();
				auto& vao = meshSource.GetVertexArray();

				// Ensure the instance buffer exists for the mesh source
				auto instanceBuffer = GetInstanceBuffer(meshSrc);
				// Update the instance buffer with the latest data
				//instanceBuffer->SetData(instances.data(), instances.size() * sizeof(InstanceData));

				// Iterate through submeshes for rendering
				for (size_t submeshIndex = 0; submeshIndex < submeshes.size(); ++submeshIndex) {

					std::vector<InstanceData> submeshInstances;
					for (const auto& instance : instances) {
						if (instance.submeshIdx == submeshIndex) { // Match submeshIdx
							submeshInstances.push_back(instance.data);
						}
					}
					if (submeshInstances.empty()) continue;
					instanceBuffer->SetData(submeshInstances.data(), static_cast<unsigned>(submeshInstances.size() * sizeof(InstanceData)));
					auto const& submesh = submeshes[submeshIndex];


					// Use glDrawElementsInstancedBaseVertexBaseInstance for rendering
					RenderAPI::DrawIndicesInstancedBaseVertexBaseInstance(
						vao,
						submesh.idxCount,           // Index count for this submesh
						static_cast<unsigned>(submeshInstances.size()), // Total instances
						submesh.baseIdx,            // Index offset
						submesh.baseVtx,            // Base vertex
						0                // Offset in the instance buffer
					);
				}
			}

			// Clear the rendered instances
			mData.instanceBufferDataMap = std::move(overflow);
		}
	}

	void Renderer::RenderSubmeshInstances(std::vector<InstanceData> const& instances,
		IGE::Assets::GUID const& meshSrc,
		size_t submeshIndex) {
		if (instances.empty()) return;

		auto const& meshSource = IGE_REF(IGE::Assets::ModelAsset, meshSrc)->mMeshSource;
		// Access the submesh and VAO
		const auto& vao = meshSource.GetVertexArray();
		const auto& submesh = meshSource.GetSubmeshes()[submeshIndex];

		// Update instance buffer
		auto instanceBuffer = GetInstanceBuffer(meshSrc);
		instanceBuffer->SetData(instances.data(), static_cast<unsigned>(instances.size() * sizeof(InstanceData)));

		// Issue the draw call
		RenderAPI::DrawIndicesInstancedBaseVertexBaseInstance(
			vao,
			submesh.idxCount,               // Index count
			static_cast<unsigned>(instances.size()), // Number of instances
			submesh.baseIdx,                // Index offset
			submesh.baseVtx,                // Base vertex offset
			0                               // Instance offset
		);

		++mData.stats.drawCalls;
	}

	int Renderer::PickEntity(glm::vec2 const& mousePos, glm::vec2 const& vpStart = {}, glm::vec2 const& vpSize = {}) {
		auto const& geomPass { Renderer::GetPass<GeomPass>() };

		auto const& pickFb{ geomPass->GetGameViewFramebuffer() };

		if (!pickFb) {
#ifdef _DEBUG
			std::cout << "ERROR: PICK FRAMEBUFFER IS NULL!" << std::endl;
#endif
			return INVALID_ENTITY_ID;
		}

		auto const& fbSpec {pickFb->GetFramebufferSpec() };

		glm::vec2 viewportStart(0.0f, 0.0f); // Game view typically starts at (0, 0)
		glm::vec2 viewportSize(static_cast<float>(fbSpec.width), static_cast<float>(fbSpec.height));

		// Ensure the mouse position is within the viewport bounds
		if (mousePos.x < viewportStart.x || mousePos.x >(viewportStart.x + viewportSize.x) ||
			mousePos.y < viewportStart.y || mousePos.y >(viewportStart.y + viewportSize.y)) {
			// Return an invalid entity if mouse is outside the viewport
			return INVALID_ENTITY_ID;
		}

		// Calculate the mouse offset relative to the viewport's top-left corner
		glm::vec2 offset = mousePos - viewportStart;

		// Normalize the offset to a [0, 1] range based on the viewport size
		float normalizedX = offset.x / viewportSize.x; // X-coordinate in [0, 1]
		float normalizedY = offset.y / viewportSize.y; // Y-coordinate in [0, 1]

		// Map normalized coordinates to framebuffer pixel coordinates
		int framebufferX = static_cast<int>(normalizedX * fbSpec.width);
		int framebufferY = static_cast<int>((1.0f - normalizedY) * fbSpec.height); // Flip Y-axis for framebuffer space

		// Bind the picking framebuffer and read the pixel under the mouse position
		pickFb->Bind();
		int entityId = pickFb->ReadPixel(1, framebufferX, framebufferY); // Read from color attachment 1
		pickFb->Unbind();

		if (entityId > 0) {
			return entityId;
		}

		return INVALID_ENTITY_ID;
	}

	ECS::Entity Renderer::PickUIEntity(glm::vec2 const& mousePos, std::vector<ECS::Entity> const& entities){
		ECS::Entity closestEntity{};
		float closestZ = std::numeric_limits<float>::lowest();

		// Iterate through all UI entities
		for (ECS::Entity const& entity : entities) {
			if (!entity.HasComponent<Component::Transform>()) continue;

			auto const& transform = entity.GetComponent<Component::Transform>();

			glm::vec4 bounds = Renderer::mUICamera.GetOrthographicBounds();
			auto const& fb = Renderer::GetPass<UIPass>()->GetTargetFramebuffer()->GetFramebufferSpec();

			glm::vec2 canvasMousePos = ConvertMouseToCanvasSpace(mousePos, bounds, glm::vec2{fb.width,fb.height});

			glm::vec2 min = glm::vec2(transform.worldPos.x - transform.worldScale.x * 0.5f,
				transform.worldPos.y - transform.worldScale.y * 0.5f);
			glm::vec2 max = glm::vec2(transform.worldPos.x + transform.worldScale.x * 0.5f,
				transform.worldPos.y + transform.worldScale.y * 0.5f);

			if (IsPointInsideBounds(canvasMousePos, min, max)) {
				if (transform.worldPos.z > closestZ) {
					closestEntity = entity;
					closestZ = transform.worldPos.z; // Update the closest Z value
				}
			}
		}
		return closestEntity;
	}

	void Renderer::FlushBatch() {
		if (mData.lineVtxCount > RendererData::cMaxVertices2D) {
#ifdef _DEBUG
			std::cerr << "Error: Line vertex count exceeds buffer capacity during FlushBatch!" << std::endl;
#endif
			mData.lineVtxCount = RendererData::cMaxVertices2D; // Clamp to valid range
		}

		if (mData.quadIdxCount) {
			RenderAPI::SetBackCulling(false);
			//ptrdiff_t difference{ reinterpret_cast<unsigned char*>(mData.quadBufferPtr)
			//					- reinterpret_cast<unsigned char*>(mData.quadBuffer.data()) };

			//how many elements it takes up in terms of bytes
			//unsigned int dataSize = static_cast<unsigned int>(difference);

			unsigned int dataSize = static_cast<unsigned int>(mData.quadBufferIndex * sizeof(QuadVtx));

			mData.quadVertexBuffer->SetData(mData.quadBuffer.data(), dataSize);

			//Bind all the textures that has been set
			for (unsigned int i{}; i < mData.texUnitIdx; ++i) {
				mData.texUnits[i].Bind(i);
			}

			//IGE_ASSETMGR.GetAsset<IGE::Assets::TextureAsset>(GetWhiteTexture())->mTexture.Bind(0);
			

			ShaderLibrary::Get("Tex2D")->Use();
			RenderAPI::DrawIndices(mData.quadVertexArray, mData.quadIdxCount);

			RenderAPI::SetBackCulling(true);

			++mData.stats.drawCalls;
		}
		if (mData.lineVtxCount) {

			unsigned int dataSize = static_cast<unsigned int>(mData.lineBufferIndex * sizeof(LineVtx));

			mData.lineVertexBuffer->SetData(mData.lineBuffer.data(), dataSize);

			auto const& lineShader = ShaderLibrary::Get("Line");
			lineShader->Use();
			auto depthTex = Renderer::GetPass<GeomPass>()->GetDepthTexture();
			if (depthTex)
				lineShader->SetUniform("u_DepthTex", depthTex, 0);
			RenderAPI::DrawLines(mData.lineVertexArray, mData.lineVtxCount);

			++mData.stats.drawCalls;
		}
		if (mData.triVtxCount) {

			unsigned int dataSize = static_cast<unsigned int>(mData.triBufferIndex * sizeof(LineVtx));

			mData.triVertexBuffer->SetData(mData.triBuffer.data(), dataSize);

			ShaderLibrary::Get("Line")->Use();

			RenderAPI::DrawTriangles(mData.triVertexArray, mData.triVtxCount);

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
				mData.texUnits[i].Bind(i);
			}

			// Use the appropriate shader and draw the indexed meshes
			ShaderLibrary::Get("Tex")->Use();
			RenderAPI::DrawIndices(mData.meshVertexArray, mData.meshIdxCount);

			// Increment draw call stats
			++mData.stats.drawCalls;
		}
	}

	void Renderer::FlushBatch(std::shared_ptr<RenderPass> const& renderPass) {

		if (mData.triVtxCount) {

			unsigned int dataSize = static_cast<unsigned int>(mData.triBufferIndex * sizeof(LineVtx));

			mData.triVertexBuffer->SetData(mData.triBuffer.data(), dataSize);

			RenderAPI::DrawTriangles(mData.triVertexArray, mData.triVtxCount);

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
				mData.texUnits[i].Bind(i);
			}

			RenderAPI::DrawIndices(mData.meshVertexArray, mData.meshIdxCount);

			// Increment draw call stats
			++mData.stats.drawCalls;
		}
	}

	void Renderer::BeginBatch() {
		mData.quadIdxCount = 0;
		mData.triVtxCount = 0;
		mData.lineVtxCount = 0;
		//mData.quadBufferPtr = mData.quadBuffer.data();
		mData.quadBufferIndex = 0;  // Reset the index for the new batch
		mData.triBufferIndex = 0;
		mData.lineBufferIndex = 0;

		mData.texUnitIdx = 1;

		// Reset for general meshes
		mData.meshIdxCount = 0;
		mData.meshVtxCount = 0;
		mData.meshIdxBuffer.clear();

	}

	bool Renderer::IsPointInsideBounds(glm::vec2 const& point, glm::vec2 const& min, glm::vec2 const& max)
	{
		if (point.x < min.x || point.x > max.x || point.y < min.y || point.y > max.y)
			return false;
		return true;
	}

	void Renderer::NextBatch() {
		FlushBatch();
		BeginBatch();
	}

	void Renderer::RenderSceneBegin(glm::mat4 const& viewProjMtx, CameraSpec const& cam) {

		auto const& lineShader = ShaderLibrary::Get("Line");
		lineShader->Use();
		lineShader->SetUniform("u_ViewProjMtx", viewProjMtx);
		auto depthFb = Renderer::GetPass<GeomPass>()->GetTargetFramebuffer();

		lineShader->SetUniform("u_ScreenSize", glm::vec2{depthFb->GetFramebufferSpec().width, depthFb->GetFramebufferSpec().height});
		lineShader->SetUniform("u_FarPlane", cam.farClip);

		//mData.lineShader->SetUniform("u_ViewProjMtx", viewProjMtx);
		auto const& texShader = ShaderLibrary::Get("Tex2D");
		texShader->Use();
		texShader->SetUniform("u_ViewProjMtx", viewProjMtx);

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

	Statistics const& Renderer::GetStats() {
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

	std::shared_ptr<Graphics::Framebuffer> const& Renderer::GetFinalFramebuffer() {
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
	void Renderer::ResizeFinalFramebuffer(int width, int height){
		mFinalFramebuffer->Resize(width, height);

	}
	IGE::Assets::GUID Renderer::GetDebugMeshSource(size_t idx){
		return mData.debugMeshSources[idx];
	}

	IGE::Assets::GUID Renderer::GetQuadMeshSource() {
		return mData.quadMeshSource;
	}
}