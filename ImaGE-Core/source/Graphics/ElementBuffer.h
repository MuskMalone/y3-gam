/*!*********************************************************************
\file   ElementBuffer.h
\author k.choa\@digipen.edu
\date   22/09/2024
\brief  The ElementBuffer class encapsulates the functionality for creating, binding,
		and managing an OpenGL Element Buffer Object (EBO), which is used to store
		indices for rendering operations. It supports dynamic and static buffer usage.

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#pragma once
#include <memory>

namespace Graphics{
	class ElementBuffer {
	public:
		ElementBuffer(unsigned int size);
		ElementBuffer(unsigned int const* indices, unsigned int count);
		~ElementBuffer();

		static std::shared_ptr<ElementBuffer> Create(unsigned int size);
		static std::shared_ptr<ElementBuffer> Create(unsigned int const* indices, unsigned int count);

		void SetData(void* const data, unsigned int size);

		void Bind() const;
		void Unbind() const;
		unsigned int GetCount() const;
	private:
		unsigned int mEboHdl;
		unsigned int mIdxCount;
	};
}
