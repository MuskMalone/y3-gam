#include <pch.h>
#include "Pipeline.h"
#include "Framebuffer.h"

namespace Graphics {

	Pipeline::Pipeline(const PipelineSpec& spec) {
		mSpec = spec;
	}

	PipelineSpec& Pipeline::GetSpec() { return mSpec; }
	PipelineSpec const& Pipeline::GetSpec() const { return mSpec; }

	std::shared_ptr<Shader> Pipeline::GetShader() const { return mSpec.shader; }

	std::shared_ptr<Pipeline> Pipeline::Create(const PipelineSpec& spec) {
		return std::make_shared<Pipeline>(spec);
	}

	void Pipeline::Bind() {
		if (mSpec.targetFramebuffer)
			mSpec.targetFramebuffer->Bind();  // Bind custom framebuffer if it exists
		else
			Framebuffer::BindDefault();  // Bind the default framebuffer

		mSpec.shader->Use();  // Bind the shader

		// Set OpenGL states based on spec @TODO might handle states more efficiently in the future (less state changes)
		if (mSpec.backfaceCulling) {
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
		}
		else {
			glDisable(GL_CULL_FACE);
		}

		if (mSpec.depthTest) {
			glEnable(GL_DEPTH_TEST);
		}
		else {
			glDisable(GL_DEPTH_TEST);
		}

		if (mSpec.wireframe) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

		glLineWidth(mSpec.lineWidth);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void Pipeline::Unbind() {
		if (mSpec.targetFramebuffer) {
			mSpec.targetFramebuffer->Unbind();  // Unbind custom framebuffer
		}
		else {
			Framebuffer::BindDefault();  // Bind the default framebuffer when done
		}

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); //make sure not in wireframe mode
	}
}