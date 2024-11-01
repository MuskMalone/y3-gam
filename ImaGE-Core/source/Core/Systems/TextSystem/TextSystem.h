#pragma once
#include <Core/Systems/System.h>
#include <Core/Systems/SystemManager/SystemManager.h>

#include <Graphics/Texture.h>
#include <Graphics/VertexArray.h>
#include <Graphics/VertexBuffer.h>
#include <Graphics/ElementBuffer.h>
#include <Graphics/Shader.h>

#include "Events/EventManager.h"
#include "Events/EventCallback.h"

#include "ft2build.h"
#include FT_FREETYPE_H

namespace Systems {

	struct FontVtx {
		glm::vec2 pos;
		glm::vec2 texCoord;
	};

	struct Character {
		float uMin;
		float uMax;
		float vMin;
		float vMax;
		glm::ivec2   size;
		glm::ivec2   bearing; // Offset from baseline to left/top of glyph
		unsigned int advance; // Offset to advance to next glyph
	};

	struct FaceObject {
		FT_Face face{};
		std::shared_ptr<Graphics::VertexArray> vao;
		std::shared_ptr<Graphics::VertexBuffer> vbo;
		std::shared_ptr<Graphics::ElementBuffer> ebo;
	};

	struct Font {
		Font() = delete;
		Font(std::string const& filePath);

		std::string mFilePath;
		uint32_t mFilePathHash;
		FaceObject mFace;
		std::map<char, Character> mCharacterMap;
		std::shared_ptr<Graphics::Texture> mBitmap;
	};

	class TextSystem : public System {
	public:
		TextSystem(const char* name);

		void Start() override;
		void Update() override;
		void Destroy() override;

		void AddFont(Systems::Font& font);
		std::unordered_map<uint32_t, std::shared_ptr<Systems::Font>> const& GetLoadedFontMap() const;
		void RenderText(uint32_t filePathHash, std::string textContent,
			float xPos, float yPos, float scale, glm::vec3 color);
		float GetTextWidth(uint32_t filePathHash, std::string textContent, float scale);
		bool IsValid(Font const& font) const;

	private:
		const int MAX_ASCII{ 128 };
		const unsigned int MAX_QUADS{ 1000 };
		const unsigned int MAX_VERTICES{ MAX_QUADS * 4 };
		const unsigned int MAX_INDICES{ MAX_QUADS * 6 };
		const unsigned int DEFAULT_FONT_SIZE{ 100 };

	private:
		void LoadFontFace(Systems::Font& font) const;
		void SetFontSize(uint32_t filePathHash, int fontSize);
		void GenerateBitmap(uint32_t filePathHash, int fontSize);
		void CreateGLObjects(uint32_t filePathHash);
		void DebugGlyph(uint32_t filePathHash, int width, int height, unsigned char* buffer);

		EVENT_CALLBACK_DECL(OnSceneLoad);

	private:
		FT_Library mFreeTypeLib;
		std::unordered_map<uint32_t, std::shared_ptr<Systems::Font>> mFonts;
		std::shared_ptr<Graphics::Shader> mShader;
	};

}
