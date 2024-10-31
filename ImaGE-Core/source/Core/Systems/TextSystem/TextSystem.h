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

		const std::string mFilePath;
		const uint32_t mFilePathHash;
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

		void AddFont(uint32_t filePathHash);

	private:
		EVENT_CALLBACK_DECL(OnSceneLoad);

	private:
		FT_Library mFreeTypeLib;
		std::unordered_map<uint32_t, Systems::Font> mFonts;
		std::shared_ptr<Graphics::Shader> mShader;
	};

}
