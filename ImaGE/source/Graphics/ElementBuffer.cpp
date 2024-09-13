#include "pch.h"
#include "ElementBuffer.h"

namespace Graphics {
	/*  _________________________________________________________________________ */
	/*! ElementBuffer

	@param indices
	Pointer to the indices data.

	@param count
	The number of indices.

	This constructor initializes the ElementBuffer with the provided indices data
	and count and sets the buffer data storage with static draw usage.
	*/
	ElementBuffer::ElementBuffer(unsigned int* indices, unsigned int count) {
		glCreateBuffers(1, &mEboHdl);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEboHdl);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), indices, GL_STATIC_DRAW);
		mIdxCount = count;

	}

	/*  _________________________________________________________________________ */
	/*! ~ElementBuffer

	Destructor for the ElementBuffer class. Cleans up the EBO resources.
	*/
	ElementBuffer::~ElementBuffer() {
		glDeleteBuffers(1, &mEboHdl);
	}

	/*  _________________________________________________________________________ */
	/*! Create

	@param indices
	Pointer to the indices data.

	@param count
	The number of indices.

	@return
	A shared pointer to the created ElementBuffer.

	This static function creates a new ElementBuffer with the provided indices data
	and count.
	*/
	std::shared_ptr<ElementBuffer> ElementBuffer::Create(unsigned int* indices, unsigned int count) {
		return std::make_shared<ElementBuffer>(indices, count);
	}

	/*  _________________________________________________________________________ */
	/*! Bind

	Binds the ElementBuffer to the GL_ELEMENT_ARRAY_BUFFER target.
	*/
	void ElementBuffer::Bind() const {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEboHdl);
	}

	/*  _________________________________________________________________________ */
	/*! Unbind

	Unbinds the ElementBuffer from the GL_ELEMENT_ARRAY_BUFFER target.
	*/
	void ElementBuffer::Unbind() const {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	/*  _________________________________________________________________________ */
	/*! GetCount

	@return
	The number of indices in the ElementBuffer.

	This function returns the number of indices stored in the ElementBuffer.
	*/
	unsigned int ElementBuffer::GetCount() const {
		return mIdxCount;
	}
}