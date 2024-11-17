#include <pch.h>
#include "RenderAPI.h"
#include "Utils.h"

namespace Graphics {
	void RenderAPI::Init(){
		GLCALL(glEnable(GL_BLEND));
		GLCALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

		GLCALL(glEnable(GL_DEPTH_TEST));
		GLCALL(glEnable(GL_LINE_SMOOTH));
		GLCALL(glHint(GL_LINE_SMOOTH_HINT, GL_NICEST));

	}

	void RenderAPI::SetBackCulling(bool b){
		if (b) {
			GLCALL(glEnable(GL_CULL_FACE));
		}
		else {
			GLCALL(glDisable(GL_CULL_FACE));
		}
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

	void RenderAPI::DrawIndicesInstancedBaseVertexBaseInstance(std::shared_ptr<VertexArray> const& vao, uint32_t idxCount, uint32_t instanceCount, uint32_t indexOffset, int baseVertex, int baseInstance){
		vao->Bind();
		int32_t count = idxCount ? idxCount : vao->GetElementBuffer()->GetCount();
		//GLCALL(glDrawElementsInstancedBaseVertexBaseInstance(
		//	GL_TRIANGLES,                    // mode: specifies what kind of primitives to render
		//	count,                           // count: number of elements to render
		//	GL_UNSIGNED_INT,                 // type: specifies the type of values in the indices array (unsigned int here)
		//	reinterpret_cast<void*>(indexOffset * sizeof(uint32_t)),  // indices: offset to the first index of the submesh in EBO
		//	instanceCount,                   // primcount: number of instances of geometry to render
		//	baseVertex,
		//	baseInstance// basevertex: constant added to each index before fetching vertex
		//));

		GLCALL(glDrawElementsInstancedBaseVertex(
			GL_TRIANGLES,                    // mode: specifies what kind of primitives to render
			count,                           // count: number of elements to render
			GL_UNSIGNED_INT,                 // type: specifies the type of values in the indices array (unsigned int here)
			reinterpret_cast<void*>(indexOffset * sizeof(uint32_t)),  // indices: offset to the first index of the submesh in EBO
			instanceCount,                   // primcount: number of instances of geometry to render
			baseVertex
			//baseInstance// basevertex: constant added to each index before fetching vertex
		));
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