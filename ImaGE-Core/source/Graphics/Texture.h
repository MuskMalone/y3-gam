#pragma once
#include <string>

namespace Graphics {

	class Texture {
	public:

		static std::shared_ptr<Texture> Create(std::string const& path, bool isBindless = false);
		//Temp default constructor
		Texture();
		Texture(std::string const& path, bool isBindless = false);
		Texture(uint32_t width, uint32_t height, bool isBindless = false);

		// For Font Glyphs
		Texture(unsigned int width, unsigned int height, const void* data);

		~Texture();

		uint32_t GetWidth() const;
		uint32_t GetHeight() const;
		uint32_t GetTexHdl() const;
		GLuint64 GetBindlessHandle() const;

		bool IsBindless() const;

		void SetData(void* data);

		void Bind(unsigned int texUnit = 0) const;
		void Unbind(unsigned int texUnit = 0) const;

		bool operator==(Texture const& rhs) const;
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
