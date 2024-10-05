/*!*********************************************************************
\file   VertexArray.h
\author k.choa\@digipen.edu
\date   22/09/2024
\brief  The VertexArray class manages the setup of vertex attribute pointers and the binding of VertexBuffers
		and ElementBuffers to the VAO. It simplifies the process of configuring vertex input attributes 
		for rendering.

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#pragma once

#include "VertexBuffer.h"
#include "ElementBuffer.h"

namespace Graphics {

	class VertexArray{
	public:
		VertexArray();
		~VertexArray();

		static std::shared_ptr<VertexArray> Create();

		void Bind() const;
		void Unbind() const;

		void AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vbo, bool isInstanced = false);
		void SetElementBuffer(const std::shared_ptr<ElementBuffer>& ebo);

		std::vector<std::shared_ptr<VertexBuffer>> const& GetVertexBuffers() const { return mVbos; }
		std::shared_ptr<ElementBuffer> const& GetElementBuffer() const { return mEbo; }

	private:
		uint32_t vaoHdl;
		uint32_t mAttribIdx{};
		std::vector<std::shared_ptr<VertexBuffer>> mVbos;
		std::shared_ptr<ElementBuffer> mEbo;
	};

}

