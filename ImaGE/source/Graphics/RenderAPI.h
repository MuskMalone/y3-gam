#pragma once
#include "VertexArray.h"

namespace Graphics{
	class RenderAPI{
	public:
		void Init();
		void SetViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height);
		void SetClearColor(const glm::vec4& color);
		void Clear();

		void static DrawIndices(std::shared_ptr<VertexArray> const& vao, unsigned int idxCount);
		void DrawLines(std::shared_ptr<VertexArray> const& vao, unsigned int vtxCount);

		void SetLineWidth(float width);
	};
}

