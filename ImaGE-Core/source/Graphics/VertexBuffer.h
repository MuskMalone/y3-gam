/*!*********************************************************************
\file   VertexBuffer.h
\author k.choa\@digipen.edu
\date   22/09/2024
\brief  The VertexBuffer class simplifies the process of creating, binding, and managing
		vertex buffers for use in rendering pipelines. It supports both dynamic and static
		vertex buffer initialization and allows data to be set or updated after creation.

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#pragma once
#include <memory>
#include <string>

namespace Graphics{
	enum class AttributeType {
		NONE = 0,
		FLOAT,
		VEC2,
		VEC3,
		VEC4,
		MAT3,
		MAT4,
		UINT,
		UVEC2,
		UVEC3,
		UVEC4,
		INT,
		IVEC2,
		IVEC3,
		IVEC4,
		BOOL
	};

	static unsigned int AttributeTypeSize(AttributeType type)
	{
		switch (type) {
		case AttributeType::FLOAT:	return 4;
		case AttributeType::VEC2:	return 4 * 2;
		case AttributeType::VEC3:	return 4 * 3;
		case AttributeType::VEC4:	return 4 * 4;
		case AttributeType::MAT3:	return 4 * 3 * 3;
		case AttributeType::MAT4:	return 4 * 4 * 4;
		case AttributeType::INT:	return 4;
		case AttributeType::UINT:   return 4;
		case AttributeType::UVEC2: return 4 * 2;
		case AttributeType::UVEC3: return 4 * 3;
		case AttributeType::UVEC4: return 4 * 4;
		case AttributeType::IVEC2:	return 4 * 2;
		case AttributeType::IVEC3:	return 4 * 3;
		case AttributeType::IVEC4:	return 4 * 4;
		case AttributeType::BOOL:	return 1;
		}

		std::cout << "ERROR: Unknown AttributeType!\n";
		//@TODO exit program or assert
		return 0;
	}

	static std::string AttributeTypeToString(AttributeType type)
	{
		switch (type) {
		case AttributeType::FLOAT:  return "FLOAT";
		case AttributeType::VEC2:   return "VEC2";
		case AttributeType::VEC3:   return "VEC3";
		case AttributeType::VEC4:   return "VEC4";
		case AttributeType::MAT3:   return "MAT3";
		case AttributeType::MAT4:   return "MAT4";
		case AttributeType::UINT:   return "UINT";
		case AttributeType::UVEC2:  return "UVEC2";
		case AttributeType::UVEC3:  return "UVEC3";
		case AttributeType::UVEC4:  return "UVEC4";
		case AttributeType::INT:    return "INT";
		case AttributeType::IVEC2:  return "IVEC2";
		case AttributeType::IVEC3:  return "IVEC3";
		case AttributeType::IVEC4:  return "IVEC4";
		case AttributeType::BOOL:   return "BOOL";
		default: return "UNKNOWN";
		}
	}

	struct BufferAttribute {
		std::string name;
		AttributeType type;
		unsigned int size;
		unsigned int offset;
		bool isNormalized;

		BufferAttribute() = default;

		/*  _________________________________________________________________________ */
		/*! BufferAttribute

		@param b_type
		The type of the attribute.

		@param b_name
		The name of the attribute.

		@param b_isNormalized
		Whether the attribute is normalized.

		This constructor initializes the BufferAttribute with the provided type, name, and normalization flag.
		*/
		BufferAttribute(AttributeType b_type, std::string const& b_name, bool b_isNormalized = false)
			: name{ b_name }, type{ b_type }, size{ AttributeTypeSize(b_type) }, offset{ 0 }, isNormalized{ b_isNormalized } {

		}

		/*  _________________________________________________________________________ */
		/*! GetComponentCount

		@return
		The number of components in the attribute.

		This function returns the number of components based on the attribute type.
		*/
		unsigned int GetComponentCount() const {
			switch (type) {
			case AttributeType::FLOAT:	return 1;
			case AttributeType::VEC2:	return 2;
			case AttributeType::VEC3:	return 3;
			case AttributeType::VEC4:	return 4;
			case AttributeType::MAT4:   return 4;
			case AttributeType::UINT:   return 1;
			case AttributeType::UVEC2:	return 2;
			case AttributeType::UVEC3:	return 3;
			case AttributeType::UVEC4:	return 4;
			case AttributeType::INT:	return 1;
			case AttributeType::IVEC2:	return 2;
			case AttributeType::IVEC3:	return 3;
			case AttributeType::IVEC4:	return 4;
			case AttributeType::BOOL:	return 1;
			}

			std::cout << "ERROR: Unknown AttributeType!\n";
			//@TODO EXIT
			return 0;
		}
	};

	class BufferLayout {
	public:
		BufferLayout() {};
		BufferLayout(std::initializer_list<BufferAttribute> const& b_elements)
			: mElements(b_elements) {
			ComputeOffsetAndStride();
		}
		inline unsigned int GetStride() const { return mStride; };
		inline const std::vector<BufferAttribute>& GetElements() const { return mElements; }
		std::vector<BufferAttribute>::iterator begin() { return mElements.begin(); };
		std::vector<BufferAttribute>::iterator end() { return mElements.end(); };
		std::vector<BufferAttribute>::const_iterator begin() const { return mElements.begin(); };
		std::vector<BufferAttribute>::const_iterator end() const { return mElements.end(); };

		void PrintLayout() const {
			std::cout << "Buffer Layout: \n";
			for (const auto& elem : mElements) {
				std::cout << "  Attribute: " << elem.name
					<< ", Type: " << AttributeTypeToString(elem.type)
					<< ", Size: " << elem.size
					<< ", Offset: " << elem.offset
					<< ", Normalized: " << (elem.isNormalized ? "true" : "false")
					<< "\n";
			}
			std::cout << "Total Stride: " << mStride << " bytes\n";
		}
	private:
		void ComputeOffsetAndStride() {
			unsigned int newOffset{ 0 };
			mStride = 0;

			for (auto& elem : mElements) {
				elem.offset = newOffset;
				newOffset += elem.size;
				mStride += elem.size;
			}
			
		}
	private:
		std::vector<BufferAttribute> mElements;
		unsigned int mStride{};
	};

#pragma once
	class VertexBuffer {
	public:
		VertexBuffer(unsigned int size);
		VertexBuffer(float* vertices, unsigned int size);
		~VertexBuffer();

		static std::shared_ptr<VertexBuffer> Create(unsigned int size);
		static std::shared_ptr<VertexBuffer> Create(float* vertices, unsigned int size);

		void Bind() const;
		void Unbind() const;

		BufferLayout const& GetLayout() const;
		void SetLayout(BufferLayout& layout);

		void SetData(void const* data, unsigned int size);

	private:
		unsigned int mVboHdl;
		BufferLayout mLayout;
	};
}
