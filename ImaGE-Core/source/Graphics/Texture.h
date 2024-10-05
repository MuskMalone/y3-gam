/*!*********************************************************************
\file   Texture.h
\author k.choa\@digipen.edu
\date   22/09/2024
\brief  The Texture class is responsible for creating and managing 2D textures in OpenGL. It supports
		loading textures from files, creating textures with specific dimensions, and setting texture data.
		The class also manages texture binding, unbinding, and provides utilities for texture comparison.

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#pragma once
#include <string>
#include <Asset/SmartPointer.h>
namespace Graphics {

	class Texture {
	public:

		static IGE::Assets::GUID Create(std::string const& path);
		//Temp default constructor
		Texture();
		Texture(std::string const& path);
		Texture(unsigned int width, unsigned int height);

		// For Font Glyphs
		Texture(unsigned int width, unsigned int height, const void* data);

		~Texture();

		unsigned int GetWidth() const;
		unsigned int GetHeight() const;
		unsigned int GetTexHdl() const;

		void SetData(void* data);

		void Bind(unsigned int texUnit = 0) const;
		void Unbind(unsigned int texUnit = 0) const;

		bool operator==(Texture const& rhs) const;
	private:
		std::string mPath;
		unsigned int mWidth;
		unsigned int mHeight;
		unsigned int mTexHdl;
	};
}	// namespace Graphics
