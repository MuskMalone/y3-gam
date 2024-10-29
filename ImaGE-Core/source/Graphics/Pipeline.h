/*!*********************************************************************
\file   Pipeline.h
\author k.choa\@digipen.edu
\date   22/09/2024
\brief  The Pipeline class encapsulates the setup and configuration of the rendering pipeline. It manages
		the OpenGL state, such as backface culling, depth testing, wireframe mode, and framebuffer bindings.
		The class also provides access to the shader and pipeline specifications.

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#pragma once
#include "RenderCommand.h"
#include "Shader.h"
#include "VertexBuffer.h"
#include "Framebuffer.h"

namespace Graphics {
	enum class PrimitiveTopology
	{
		NONE = 0,
		POINTS,
		LINES,
		TRIANGLES,
		LINE_STRIP,
		TRI_STRIP,
		TRI_FAN
	};

	struct PipelineSpec
	{
		std::shared_ptr<Shader> shader;
		std::shared_ptr<Framebuffer> targetFramebuffer;
		BufferLayout layout;
		BufferLayout instanceLayout;
		BufferLayout boneInfluenceLayout;
		PrimitiveTopology Topology = PrimitiveTopology::TRIANGLES;
		bool backfaceCulling = true;
		bool depthTest = true;
		bool depthWrite = true;
		bool wireframe = false;
		float lineWidth = 1.0f;

		std::string DebugName;
	};

	class Pipeline {
	public:
		Pipeline(const PipelineSpec& spec);
		~Pipeline() = default;

		PipelineSpec& GetSpec();
		PipelineSpec const& GetSpec() const;

		std::shared_ptr<Shader> GetShader() const;
		static std::shared_ptr<Pipeline> Create(const PipelineSpec& spec);

		void Bind();
		void Unbind();
	private:
		PipelineSpec mSpec;

	};
}