/*!*********************************************************************
\file   RenderAPI.h
\author k.choa\@digipen.edu
\date   22/09/2024
\brief  The RenderAPI class manages common OpenGL rendering functions such as setting up the viewport,
		clearing buffers, and drawing indexed and instanced geometry. It also handles state management
		for blending, depth testing, and line smoothing.

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#pragma once
#include "VertexArray.h"

namespace Graphics{
	class RenderAPI{
	public:
		void Init();
		static void SetBackCulling(bool b);
		static void SetViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height);
		static void SetClearColor(const glm::vec4& color);
		static void Clear();

		static void DrawIndicesInstancedBaseVertexBaseInstance(std::shared_ptr<VertexArray> const& vao,
												   uint32_t idxCount,
												   uint32_t instanceCount,
												   uint32_t indexOffset,
												   int baseVertex,
												   int baseInstance);
		static void DrawIndicesInstanced(std::shared_ptr<VertexArray> const& vao, unsigned int idxCount, unsigned int instanceCount);
		static void DrawIndices(std::shared_ptr<VertexArray> const& vao, unsigned int idxCount);
		static void DrawTriangles(std::shared_ptr<VertexArray> const& vao, unsigned int vtxCount);
		static void DrawLines(std::shared_ptr<VertexArray> const& vao, unsigned int vtxCount);

		static void SetLineWidth(float width);
	};
}

