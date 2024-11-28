#include <pch.h>
#include "Texture.h"
#include "Utils.h"
#include <Asset/IGEAssets.h>
#include <DirectXTex.h>
//TEMP?? 

namespace Graphics {

	/*  _________________________________________________________________________ */
	/*! Texture (default constructor)

	Default constructor for the Texture class. Initializes the texture dimensions
	and handle to default values.
	*/
	Texture::Texture() : mWidth{ 0 }, mHeight{ 0 }, mTexHdl{ 0 } {}

	Texture::Texture(uint32_t texHdl) : mWidth{}, mHeight{}, mTexHdl{ texHdl } {

	}

	/*  _________________________________________________________________________ */
	/*! Texture

	@param path
	The path to the texture file.

	This constructor initializes the Texture using the provided file path. It loads
	the image using stb_image and sets up the OpenGL texture.
	*/
	Texture::Texture(std::string const& path, bool isBindless)
		: mPath{ path }, mIsBindless{ isBindless } {
		// Load image using stb_image
		std::wstring wPath = std::wstring(path.begin(), path.end());

		// Load the DDS file using DirectXTex
		DirectX::ScratchImage image;
		HRESULT hr = DirectX::LoadFromDDSFile(wPath.c_str(), DirectX::DDS_FLAGS_NONE, nullptr, image);
		if (FAILED(hr)) {
			std::cerr << "Failed to load DDS texture: " << path << std::endl;
			throw std::runtime_error{ "failed to load dds tex"};
		}

		// Flip the image vertically
		DirectX::ScratchImage flippedImage;
		hr = DirectX::FlipRotate(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FR_FLIP_VERTICAL, flippedImage);
		if (FAILED(hr)) {
			std::cerr << "Failed to flip DDS texture vertically." << std::endl;
			throw std::runtime_error{ "Failed to flip DDS texture vertically" };
		}

		// Retrieve the flipped image data
		const DirectX::Image* img = flippedImage.GetImage(0, 0, 0);
		if (!img) {
			std::cerr << "Failed to retrieve flipped image data." << std::endl;
			throw std::runtime_error{ "Failed to retrieve flipped image data" };
		}

		mWidth = static_cast<uint32_t>(img->width);
		mHeight = static_cast<uint32_t>(img->height);

		GLint internalFormat;
		GLenum format;
		GLenum type;

		switch (img->format) {
		case DXGI_FORMAT_R8G8B8A8_UNORM:
			internalFormat = GL_RGBA8;
			format = GL_RGBA;
			type = GL_UNSIGNED_BYTE;
			break;

		case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
			internalFormat = GL_SRGB8_ALPHA8;
			format = GL_RGBA;
			type = GL_UNSIGNED_BYTE;
			break;

		case DXGI_FORMAT_B8G8R8A8_UNORM:
			internalFormat = GL_RGBA8;
			format = GL_BGRA;
			type = GL_UNSIGNED_BYTE;
			break;

		case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
			internalFormat = GL_RGBA8/*GL_SRGB8_ALPHA8*/;
			format = GL_BGRA;
			type = GL_UNSIGNED_BYTE;
			break;

		case DXGI_FORMAT_R32G32B32A32_FLOAT:
			internalFormat = GL_RGBA32F;
			format = GL_RGBA;
			type = GL_FLOAT;
			break;

		case DXGI_FORMAT_R16G16B16A16_FLOAT:
			internalFormat = GL_RGBA16F;
			format = GL_RGBA;
			type = GL_HALF_FLOAT;
			break;

		case DXGI_FORMAT_R11G11B10_FLOAT:
			internalFormat = GL_R11F_G11F_B10F;
			format = GL_RGB;
			type = GL_FLOAT;
			break;

		case DXGI_FORMAT_R32G32_FLOAT:
			internalFormat = GL_RG32F;
			format = GL_RG;
			type = GL_FLOAT;
			break;

		case DXGI_FORMAT_R32_FLOAT:
			internalFormat = GL_R32F;
			format = GL_RED;
			type = GL_FLOAT;
			break;

		case DXGI_FORMAT_R8_UNORM:
			internalFormat = GL_R8;
			format = GL_RED;
			type = GL_UNSIGNED_BYTE;
			break;

		case DXGI_FORMAT_R16_FLOAT:
			internalFormat = GL_R16F;
			format = GL_RED;
			type = GL_HALF_FLOAT;
			break;
		default:
			std::cerr << "Unsupported DXGI format! ENUM: (" << img->format <<") Using default parameters." << std::endl;
			internalFormat = GL_RGBA8;
			format = GL_RGBA;
			type = GL_UNSIGNED_BYTE;
		}

		GLCALL(glCreateTextures(GL_TEXTURE_2D, 1, &mTexHdl));
		// allocate GPU storage for texture image data loaded from file
		GLCALL(glTextureStorage2D(mTexHdl, 1, internalFormat, mWidth, mHeight));


		// Set texture parameters
		GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
		GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
		GLCALL(glTextureParameteri(mTexHdl, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
		GLCALL(glTextureParameteri(mTexHdl, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
		// copy image data from client memory to GPU texture buffer memory
		GLCALL(glTextureSubImage2D(mTexHdl, 0, 0, 0, mWidth, mHeight,
			format, type, img->pixels));

		if (mIsBindless) {
			InitBindlessTexture();
		}
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
	Texture::Texture(uint32_t width, uint32_t height, GLenum intFmt, bool isBindless) :mWidth{ width }, mHeight{ height }, mIsBindless{isBindless} {

		//TODO might add more parameters

		GLCALL(glCreateTextures(GL_TEXTURE_2D, 1, &mTexHdl));
		// allocate GPU storage for texture image data loaded from file
		GLCALL(glTextureStorage2D(mTexHdl, 1, intFmt, mWidth, mHeight));

		// Set texture parameters
		//GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
		//GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
		GLCALL(glTextureParameteri(mTexHdl, GL_TEXTURE_WRAP_S, GL_REPEAT));
		GLCALL(glTextureParameteri(mTexHdl, GL_TEXTURE_WRAP_T, GL_REPEAT));
		GLCALL(glTextureParameteri(mTexHdl, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
		GLCALL(glTextureParameteri(mTexHdl, GL_TEXTURE_MAG_FILTER, GL_NEAREST));

		if (mIsBindless) {
			InitBindlessTexture();
		}

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
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glGenTextures(1, &mTexHdl);
		glBindTexture(GL_TEXTURE_2D, mTexHdl);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			mWidth,
			mHeight,
			0,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			data
		);
		// set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 4); // reset byte-alignment
	}

	/*  _________________________________________________________________________ */
	/*! ~Texture

	Destructor for the Texture class. Cleans up the texture resources.
	*/
	Texture::~Texture() {
		if (mIsBindless && mBindlessHdl != 0) {
			//glMakeTextureHandleNonResidentARB(mBindlessHdl);
		}
		glDeleteTextures(1, &mTexHdl);
	}

	/*  _________________________________________________________________________ */
	/*! GetWidth

	@return
	The width of the texture.

	This function returns the width of the texture.
	*/
	uint32_t Texture::GetWidth() const {
		return mWidth;
	}


	/*  _________________________________________________________________________ */
	/*! GetHeight

	@return
	The height of the texture.

	This function returns the height of the texture.
	*/
	uint32_t Texture::GetHeight() const {
		return mHeight;
	}

	/*  _________________________________________________________________________ */
	/*! GetTexHdl

	@return
	The handle to the OpenGL texture.

	This function returns the handle to the OpenGL texture.
	*/
	uint32_t Texture::GetTexHdl() const {
		return mTexHdl;
	}

	GLuint64 Texture::GetBindlessHandle() const{
		return mIsBindless ? mBindlessHdl : 0;
	}

	bool Texture::IsBindless() const
	{
		return false;// mIsBindless;
	}

	/*  _________________________________________________________________________ */
	/*! SetData

	@param data
	Pointer to the texture data.

	This function sets the data for the texture.
	*/
	void Texture::SetData(void* data) {
		//TODO may add format to texture member variables
		GLCALL(glTextureSubImage2D(mTexHdl, 0, 0, 0, mWidth, mHeight, GL_RGBA, GL_UNSIGNED_BYTE, data));
	}

	void Texture::CopyFrom(GLuint srcTexHdl, GLsizei width, GLsizei height){
		if (mTexHdl) {
			GLCALL(glCopyImageSubData(srcTexHdl, GL_TEXTURE_2D, 0, 0, 0, 0,
				mTexHdl, GL_TEXTURE_2D, 0, 0, 0, 0,
				width, height, 1));
		}
	}

	/*  _________________________________________________________________________ */
	/*! Bind

	@param texUnit
	The texture unit to bind to

	This function binds the texture to the specified texture unit.
	*/
	void Texture::Bind(uint32_t texUnit) const {
		//if (mIsBindless) {
		//	Debug::DebugLogger::GetInstance().LogWarning("Bind() called on a bindless texture. This operation is not applicable.");
		//	return;
		//}

		GLCALL(glBindTextureUnit(texUnit, mTexHdl));
	}

	/*  _________________________________________________________________________ */
	/*! Unbind

	@param texUnit
	The texture unit to unbind from (default is 0).

	This function unbinds the texture from the specified texture unit.
	*/
	void Texture::Unbind(unsigned int texUnit) const {
		//if (mIsBindless) return;
		GLCALL(glBindTextureUnit(texUnit, 0));
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
		if (mIsBindless) return false;
		return mTexHdl == rhs.mTexHdl;
	}

	void Texture::InitBindlessTexture(){
		if (GLAD_GL_ARB_bindless_texture) {
			mBindlessHdl = glGetTextureHandleARB(mTexHdl);
			GLCALL(glMakeTextureHandleResidentARB(mBindlessHdl));
		}
		else {
			Debug::DebugLogger::GetInstance().LogError("Bindless textures are not supported on this system.");
			std::cout << "NOT SUPPORTED" << std::endl;
		}
	}

	IGE::Assets::GUID Texture::Create(std::string const& path, bool isBindless) {
		//return std::make_shared<Texture>(path, isBindless);
		return IGE_ASSETMGR.LoadRef<IGE::Assets::TextureAsset>(path);
	}
}