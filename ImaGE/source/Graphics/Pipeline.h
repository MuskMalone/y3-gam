#pragma once
#include <pch.h>
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