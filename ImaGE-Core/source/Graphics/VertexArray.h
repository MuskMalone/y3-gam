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

		void AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vbo);
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

