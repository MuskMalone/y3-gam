#include <pch.h>
#include "VertexArray.h"
#include "Utils.h"

namespace Graphics {
	static GLenum AttributeToGLType(AttributeType type) {
		switch (type) {
		case AttributeType::MAT3:
		case AttributeType::FLOAT:
		case AttributeType::VEC2:
		case AttributeType::VEC3:
		case AttributeType::VEC4:
		case AttributeType::MAT4:
			return GL_FLOAT;
		case AttributeType::UINT:
		case AttributeType::UVEC2:
		case AttributeType::UVEC3:
		case AttributeType::UVEC4:
			return GL_UNSIGNED_INT;
		case AttributeType::INT:
		case AttributeType::IVEC2:
		case AttributeType::IVEC3:
		case AttributeType::IVEC4:
			return GL_INT;
		case AttributeType::BOOL:
			return GL_BOOL;
		}

		std::cout << "Unknown AttributeType!\n";
		//@TODO EXIT PROGRAM/ ASSERT
		return 0;
	}

	/*  _________________________________________________________________________ */
	/*! VertexArray

	Default constructor for the VertexArray class. Initializes the VAO handle.
	*/
	VertexArray::VertexArray() {
		GLCALL(glCreateVertexArrays(1, &vaoHdl));
	}

	/*  _________________________________________________________________________ */
	/*! ~VertexArray

	Destructor for the VertexArray class. Deletes the VAO handle.
	*/
	VertexArray::~VertexArray() {
		GLCALL(glDeleteVertexArrays(1, &vaoHdl));
	}

	/*  _________________________________________________________________________ */
	/*! Create

	@return
	A shared pointer to a new VertexArray object.

	This function creates a new VertexArray object and returns a shared pointer to it.
	*/
	std::shared_ptr<VertexArray> VertexArray::Create() {
		return std::make_shared<VertexArray>();
	}

	/*  _________________________________________________________________________ */
	/*! Bind

	Binds the VAO for rendering.
	*/
	void VertexArray::Bind() const {
		GLCALL(glBindVertexArray(vaoHdl));
	}

	/*  _________________________________________________________________________ */
	/*! Unbind

	Unbinds the VAO.
	*/
	void VertexArray::Unbind() const {
		GLCALL(glBindVertexArray(0));
	}

	/*  _________________________________________________________________________ */
	/*! AddVertexBuffer

	@param vbo
	A shared pointer to the VertexBuffer object to be added to the VAO.

	This function binds the VAO and the provided VertexBuffer, sets up the vertex attributes
	according to the layout of the VertexBuffer, and adds the VertexBuffer to the list of VBOs.
	*/
	void VertexArray::AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vbo, bool isInstanced) {
		GLCALL(glBindVertexArray(vaoHdl));
		vbo->Bind();

		auto const& layout{ vbo->GetLayout() };
		for (auto const& elem : layout) {
			switch (elem.type) {
			case AttributeType::FLOAT:
			case AttributeType::VEC2:
			case AttributeType::VEC3:
			case AttributeType::VEC4:
				GLCALL(glEnableVertexAttribArray(mAttribIdx));
				GLCALL(glVertexAttribPointer(mAttribIdx,
					elem.GetComponentCount(),
					AttributeToGLType(elem.type),
					elem.isNormalized ? GL_TRUE : GL_FALSE,
					layout.GetStride(),
					reinterpret_cast<const void*>(
						static_cast<uintptr_t>(elem.offset))));
				if (isInstanced) {
					GLCALL(glVertexAttribDivisor(mAttribIdx, 1));
				}
				++mAttribIdx;
				break;
			case AttributeType::UINT:
			case AttributeType::UVEC2:
			case AttributeType::UVEC3:
			case AttributeType::UVEC4:
			case AttributeType::INT:
			case AttributeType::IVEC2:
			case AttributeType::IVEC3:
			case AttributeType::IVEC4:
			case AttributeType::BOOL:
				GLCALL(glEnableVertexAttribArray(mAttribIdx));
				GLCALL(glVertexAttribIPointer(mAttribIdx,
					elem.GetComponentCount(),
					AttributeToGLType(elem.type),
					layout.GetStride(),
					reinterpret_cast<const void*>(
						static_cast<uintptr_t>(elem.offset))));
				if (isInstanced) {
					GLCALL(glVertexAttribDivisor(mAttribIdx, 1));
				}
				++mAttribIdx;
				break;
			case AttributeType::MAT4:
				// A MAT4 is 4 VEC4 attributes
				for (int i = 0; i < 4; ++i) {
					GLCALL(glEnableVertexAttribArray(mAttribIdx + i)); // Enable attribute for each column of the matrix
					GLCALL(glVertexAttribPointer(mAttribIdx + i,
						4,  // Each column is a vec4, so 4 components
						GL_FLOAT,
						elem.isNormalized ? GL_TRUE : GL_FALSE,
						layout.GetStride(),
						reinterpret_cast<const void*>(static_cast<uintptr_t>(elem.offset + sizeof(glm::vec4) * i))));
					// Set divisor if using for instancing
					if (isInstanced) {
						GLCALL(glVertexAttribDivisor(mAttribIdx + i, 1));  // Set divisor if it's an instance attribute
					}
				}
				mAttribIdx += 4; // Increment attribute index by 4 since a MAT4 takes 4 locations
				break;
			}

		}
		mVbos.push_back(vbo);
	}

	/*  _________________________________________________________________________ */
	/*! SetElementBuffer

	@param ebo
	A shared pointer to the ElementBuffer object to be set for the VAO.

	This function binds the VAO and the provided ElementBuffer and sets the ElementBuffer for the VAO.
	*/
	void VertexArray::SetElementBuffer(const std::shared_ptr<ElementBuffer>& ebo) {
		GLCALL(glBindVertexArray(vaoHdl));
		ebo->Bind();
		mEbo = ebo;
	}
}