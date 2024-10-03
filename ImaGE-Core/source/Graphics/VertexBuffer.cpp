#include <pch.h>
#include "VertexBuffer.h"
#include "Utils.h"

namespace Graphics {
	/*  _________________________________________________________________________ */
/*! VertexBuffer

@param size
The size of the vertex buffer.

This constructor initializes the VertexBuffer with the provided size and sets
the buffer data storage with dynamic draw usage.
*/
	VertexBuffer::VertexBuffer(unsigned int size) {
		GLCALL(glCreateBuffers(1, &mVboHdl));
		VertexBuffer::Bind();
		GLCALL(glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW));

	}

	/*  _________________________________________________________________________ */
	/*! VertexBuffer

	@param vertices
	Pointer to the vertices data.

	@param size
	The size of the vertex buffer.

	This constructor initializes the VertexBuffer with the provided vertices data
	and size and sets the buffer data storage with static draw usage.
	*/
	VertexBuffer::VertexBuffer(float* vertices, unsigned int size) {
		GLCALL(glCreateBuffers(1, &mVboHdl));
		VertexBuffer::Bind();
		GLCALL(glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_DYNAMIC_DRAW));

	}

	/*  _________________________________________________________________________ */
	/*! ~VertexBuffer

	Destructor for the VertexBuffer class. Cleans up the VBO resources.
	*/
	VertexBuffer::~VertexBuffer() {
		GLCALL(glDeleteBuffers(1, &mVboHdl));
	}

	/*  _________________________________________________________________________ */
	/*! Create

	@param size
	The size of the vertex buffer.

	@return
	A shared pointer to the created VertexBuffer.

	This static function creates a new VertexBuffer with the provided size.
	*/
	std::shared_ptr<VertexBuffer> VertexBuffer::Create(unsigned int size) {
		return std::make_shared<VertexBuffer>(size);
	}

	/*  _________________________________________________________________________ */
	/*! Create

	@param vertices
	Pointer to the vertices data.

	@param size
	The size of the vertex buffer.

	@return
	A shared pointer to the created VertexBuffer.

	This static function creates a new VertexBuffer with the provided vertices data
	and size.
	*/
	std::shared_ptr<VertexBuffer> VertexBuffer::Create(float* vertices, unsigned int size) {
		return std::make_shared<VertexBuffer>(vertices, size);
	}

	/*  _________________________________________________________________________ */
	/*! Bind

	Binds the VertexBuffer to the GL_ARRAY_BUFFER target.
	*/
	void VertexBuffer::Bind() const {
		GLCALL(glBindBuffer(GL_ARRAY_BUFFER, mVboHdl));
	}

	/*  _________________________________________________________________________ */
	/*! Unbind

	Unbinds the VertexBuffer from the GL_ARRAY_BUFFER target.
	*/
	void VertexBuffer::Unbind() const {
		GLCALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
	}

	/*  _________________________________________________________________________ */
	/*! SetLayout

	@param layout
	The BufferLayout to set for the VertexBuffer.

	This function sets the layout for the VertexBuffer.
	*/
	void VertexBuffer::SetLayout(BufferLayout& layout) {
		mLayout = layout;
	}

	/*  _________________________________________________________________________ */
	/*! GetLayout

	@return
	The layout of the VertexBuffer.

	This function returns the layout of the VertexBuffer.
	*/
	BufferLayout const& VertexBuffer::GetLayout() const { return mLayout; }

	/*  _________________________________________________________________________ */
	/*! SetData

	@param data
	Pointer to the data to set.

	@param size
	The size of the data.

	This function sets the data for the VertexBuffer.
	*/
	void VertexBuffer::SetData(void* const data, unsigned int size) {

		GLCALL(glBindBuffer(GL_ARRAY_BUFFER, mVboHdl));
		GLCALL(glBufferSubData(GL_ARRAY_BUFFER, 0, size, data));
	}
}