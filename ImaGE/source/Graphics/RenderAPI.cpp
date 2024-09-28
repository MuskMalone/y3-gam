#include <pch.h>
#include "RenderAPI.h"

namespace Graphics {
	void RenderAPI::Init()
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_LINE_SMOOTH);
	}

	void RenderAPI::SetViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
	{
		glViewport(x, y, width, height);
	}

	void RenderAPI::SetClearColor(const glm::vec4& color)
	{
		glClearColor(color.r, color.g, color.b, color.a);
	}

	void RenderAPI::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void RenderAPI::DrawIndices(std::shared_ptr<VertexArray> const& vao, unsigned int idxCount)
	{
		vao->Bind();
		unsigned int count = idxCount ? idxCount : vao->GetElementBuffer()->GetCount();
		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
	}

	void RenderAPI::DrawLines(std::shared_ptr<VertexArray> const& vao, unsigned int vtxCount)
	{
		vao->Bind();
		//glDrawArrays(GL_LINES, 0, vtxCount);
		glDrawArrays(GL_TRIANGLES, 0, vtxCount);
	}

	void RenderAPI::SetLineWidth(float width)
	{
		glLineWidth(width);
	}
}