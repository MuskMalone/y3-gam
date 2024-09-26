#pragma once
#include "VertexArray.h"

namespace Graphics{
	class RenderAPI{
	public:
		void Init();
		static void SetViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height);
		static void SetClearColor(const glm::vec4& color);
		void Clear();

		static void DrawIndices(std::shared_ptr<VertexArray> const& vao, unsigned int idxCount);
		static void DrawLines(std::shared_ptr<VertexArray> const& vao, unsigned int vtxCount);

		void SetLineWidth(float width);
	};
}

