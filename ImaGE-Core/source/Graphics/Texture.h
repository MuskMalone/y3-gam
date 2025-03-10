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
#include <memory>
#include <Asset/SmartPointer.h>

namespace Graphics {

	class Texture {
	public:

		static IGE::Assets::GUID Create(std::string const& path, bool isBindless = true);

		//Temp default constructor
		Texture();
		Texture(uint32_t texHdl);
		Texture(std::string const& path, bool isBindless = false);
		Texture(uint32_t width, uint32_t height, GLenum intFmt = GL_RGBA8, bool isBindless = false);

		// For Font Glyphs
		Texture(unsigned int width, unsigned int height, const void* data);

		~Texture();

		uint32_t GetWidth() const;
		uint32_t GetHeight() const;
		uint32_t GetTexHdl() const;
		GLuint64 GetBindlessHandle() const;

		bool IsBindless() const;
		void SetData(void* data);
		void CopyFrom(GLuint srcTexHdl, GLsizei width, GLsizei height);

		void Bind(uint32_t texUnit) const;
		void Unbind(unsigned int texUnit) const;

		bool operator==(Texture const& rhs) const;

		static inline constexpr uint32_t sDefaultAlbedoUnit = 0;
		static inline constexpr uint32_t sDefaultNormalUnit = 16;
		static inline constexpr uint32_t sShadowMapTexUnit = 31;

	private:
		void InitBindlessTexture();

		std::string mPath;
		uint32_t mWidth;
		uint32_t mHeight;
		uint32_t mTexHdl;
		GLuint64 mBindlessHdl{};
		bool mIsBindless{false};
	};
}	// namespace Graphics
