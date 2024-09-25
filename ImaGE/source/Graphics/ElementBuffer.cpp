#include <pch.h>
#include "ElementBuffer.h"

namespace Graphics {

	ElementBuffer::ElementBuffer(unsigned int size) {
		glCreateBuffers(1, &mEboHdl);               // Create the buffer object
		ElementBuffer::Bind();                      // Bind the element buffer
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);  // Allocate memory for the buffer with dynamic draw
	}

	/*  _________________________________________________________________________ */
	/*! ElementBuffer

	@param indices
	Pointer to the indices data.

	@param count
	The number of indices.

	This constructor initializes the ElementBuffer with the provided indices data
	and count and sets the buffer data storage with static draw usage.
	*/
	ElementBuffer::ElementBuffer(unsigned int const* indices, unsigned int count) {
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

	std::shared_ptr<ElementBuffer> ElementBuffer::Create(unsigned int size) {
		return std::make_shared<ElementBuffer>(size);
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
	std::shared_ptr<ElementBuffer> ElementBuffer::Create(unsigned int const* indices, unsigned int count) {
		return std::make_shared<ElementBuffer>(indices, count);
	}

	void ElementBuffer::SetData(void* const data, unsigned int size) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEboHdl);  // Bind the Element Buffer
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, size, data);  // Update the buffer with new data
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