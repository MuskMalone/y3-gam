#include <pch.h>
#include "Texture.h"
#include "Utils.h"
#include <DirectXTex.h>
#include <Asset/AssetManager.h>
//TEMP?? 

namespace Graphics {

	/*  _________________________________________________________________________ */
	/*! Texture (default constructor)

	Default constructor for the Texture class. Initializes the texture dimensions
	and handle to default values.
	*/
	Texture::Texture() : mWidth{ 0 }, mHeight{ 0 }, mTexHdl{ 0 } {}

	/*  _________________________________________________________________________ */
	/*! Texture

	@param path
	The path to the texture file.

	This constructor initializes the Texture using the provided file path. It loads
	the image using stb_image and sets up the OpenGL texture.
	*/
	Texture::Texture(std::string const& path)
		: mPath{ path } {

		  // Load image using stb_image
			std::wstring wPath = std::wstring(path.begin(), path.end());

			// Load the DDS file using DirectXTex
			DirectX::ScratchImage image;
			HRESULT hr = DirectX::LoadFromDDSFile(wPath.c_str(), DirectX::DDS_FLAGS_NONE, nullptr, image);
			if (FAILED(hr)) {
				std::cerr << "Failed to load DDS texture: " << path << std::endl;
				throw std::runtime_error{ "failed to load dds tex"};
			}

			// Retrieve the image data
			const DirectX::Image* img = image.GetImage(0, 0, 0);
			if (!img) {
				std::cerr << "Failed to retrieve image data." << std::endl;
				throw std::runtime_error{ "failed to retrieve image data"};
			}
			mWidth = img->width;
			mHeight = img->height;

		  glCreateTextures(GL_TEXTURE_2D, 1, &mTexHdl);
		  // allocate GPU storage for texture image data loaded from file
		  glTextureStorage2D(mTexHdl, 1, GL_RGBA8, mWidth, mHeight);
		
		
		  // Set texture parameters
		  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		  glTextureParameteri(mTexHdl, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		  glTextureParameteri(mTexHdl, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		
		  // copy image data from client memory to GPU texture buffer memory
		  glTextureSubImage2D(mTexHdl, 0, 0, 0, mWidth, mHeight,
		   GL_RGBA, GL_UNSIGNED_BYTE, img->pixels);
		  // client memory not required since image is buffered in GPU memory
	}

	/*  _________________________________________________________________________ */
	/*! Texture

	@param width
	The width of the texture.

	@param height
	The height of the texture.

	This constructor initializes the Texture with the provided width and height.
	It sets up the OpenGL texture with the specified dimensions.
	*/
	Texture::Texture(unsigned int width, unsigned int height) :mWidth{ width }, mHeight{ height } {

		//TODO might add more parameters

		GLCALL(glCreateTextures(GL_TEXTURE_2D, 1, &mTexHdl));
		// allocate GPU storage for texture image data loaded from file
		GLCALL(glTextureStorage2D(mTexHdl, 1, GL_RGBA8, mWidth, mHeight));

		// Set texture parameters)
		GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
		GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
		GLCALL(glTextureParameteri(mTexHdl, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
		GLCALL(glTextureParameteri(mTexHdl, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	}

	/*  _________________________________________________________________________ */
	/*! Texture (for Font Glyphs)

	@param width
	The width of the texture.

	@param height
	The height of the texture.

	@param data
	Pointer to the glyph data.

	This constructor initializes the Texture for font glyphs using the provided
	width, height, and glyph data. It sets up the OpenGL texture with the specified
	dimensions and glyph data.
	*/
	// For Font Glyphs
	Texture::Texture(unsigned int width, unsigned int height, const void* data) : mWidth{ width }, mHeight{ height }, mTexHdl{} {
		GLCALL(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
		GLCALL(glGenTextures(1, &mTexHdl));
		GLCALL(glBindTexture(GL_TEXTURE_2D, mTexHdl));
		GLCALL(glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			mWidth,
			mHeight,
			0,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			data
		));
		// set texture options
		GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER));
		GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER));
		GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 4); // reset byte-alignment
	}

	/*  _________________________________________________________________________ */
	/*! ~Texture

	Destructor for the Texture class. Cleans up the texture resources.
	*/
	Texture::~Texture() {
		glDeleteTextures(1, &mTexHdl);
	}

	/*  _________________________________________________________________________ */
	/*! GetWidth

	@return
	The width of the texture.

	This function returns the width of the texture.
	*/
	unsigned int Texture::GetWidth() const {
		return mWidth;
	}


	/*  _________________________________________________________________________ */
	/*! GetHeight

	@return
	The height of the texture.

	This function returns the height of the texture.
	*/
	unsigned int Texture::GetHeight() const {
		return mHeight;
	}

	/*  _________________________________________________________________________ */
	/*! GetTexHdl

	@return
	The handle to the OpenGL texture.

	This function returns the handle to the OpenGL texture.
	*/
	unsigned int Texture::GetTexHdl() const {
		return mTexHdl;
	}

	/*  _________________________________________________________________________ */
	/*! SetData

	@param data
	Pointer to the texture data.

	This function sets the data for the texture.
	*/
	void Texture::SetData(void* data) {
		//TODO may add format to texture member variables
		glTextureSubImage2D(mTexHdl, 0, 0, 0, mWidth, mHeight, GL_RGBA, GL_UNSIGNED_BYTE, data);
	}

	/*  _________________________________________________________________________ */
	/*! Bind

	@param texUnit
	The texture unit to bind to (default is 0).

	This function binds the texture to the specified texture unit.
	*/
	void Texture::Bind(unsigned int texUnit) const {
		glBindTextureUnit(texUnit, mTexHdl);
	}

	/*  _________________________________________________________________________ */
	/*! Unbind

	@param texUnit
	The texture unit to unbind from (default is 0).

	This function unbinds the texture from the specified texture unit.
	*/
	void Texture::Unbind(unsigned int texUnit) const {
		glBindTextureUnit(texUnit, 0);
	}

	/*  _________________________________________________________________________ */
	/*! operator==

	@param rhs
	The right-hand side texture to compare with.

	@return
	True if the textures are equal, false otherwise.

	This function checks if two textures are equal.
	*/
	bool Texture::operator==(Texture const& rhs) const {
		return mTexHdl == rhs.mTexHdl;
	}

	//std::shared_ptr<Texture> 
	IGE::Assets::GUID Texture::Create(std::string const& path) {
		//return std::make_shared<Texture>(path);
		return IGE::Assets::AssetManager::GetInstance()->LoadRef<IGE::Assets::TextureAsset>(path);
	}

}