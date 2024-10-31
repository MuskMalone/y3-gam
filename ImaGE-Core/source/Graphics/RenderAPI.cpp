#include <pch.h>
#include "RenderAPI.h"
#include "Utils.h"

namespace Graphics {
	void RenderAPI::Init(){
		GLCALL(glEnable(GL_BLEND));
		GLCALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

		GLCALL(glEnable(GL_DEPTH_TEST));
		GLCALL(glEnable(GL_LINE_SMOOTH));
	}

	void RenderAPI::SetViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height){
		GLCALL(glViewport(x, y, width, height));
	}

	void RenderAPI::SetClearColor(const glm::vec4& color){
		GLCALL(glClearColor(color.r, color.g, color.b, color.a));
	}

	void RenderAPI::Clear(){
		GLCALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
	}

	void RenderAPI::DrawIndicesInstanced(std::shared_ptr<VertexArray> const& vao, unsigned int idxCount, unsigned int instanceCount) {
		vao->Bind();
		unsigned int count = idxCount ? idxCount : vao->GetElementBuffer()->GetCount();
		GLCALL(glDrawElementsInstanced(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr, instanceCount));
	}

	void RenderAPI::DrawIndices(std::shared_ptr<VertexArray> const& vao, unsigned int idxCount){
		vao->Bind();
		unsigned int count = idxCount ? idxCount : vao->GetElementBuffer()->GetCount();
		GLCALL(glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr));
	}

	void RenderAPI::DrawLines(std::shared_ptr<VertexArray> const& vao, unsigned int vtxCount){
		vao->Bind();
		GLCALL(glDrawArrays(GL_LINES, 0, vtxCount));
	}

	void RenderAPI::DrawTriangles(std::shared_ptr<VertexArray> const& vao, unsigned int vtxCount){
		vao->Bind();
		GLCALL(glDrawArrays(GL_TRIANGLES, 0, vtxCount));
	}

	void RenderAPI::SetLineWidth(float width){
		GLCALL(glLineWidth(width));
	}
}