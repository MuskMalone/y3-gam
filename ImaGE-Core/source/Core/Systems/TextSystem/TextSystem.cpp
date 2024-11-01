#include <pch.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

#include "TextSystem.h"
#include "Core/GUID.h"
#include "Core/Components/Text.h"
#include "Core/Components/Transform.h"

namespace Systems {

  Font::Font(std::string const& filePath) : mFilePath{ filePath }, 
    mFilePathHash{ IGE::Core::Fnv1a32(filePath.c_str(), filePath.size()) } {
    if (std::shared_ptr<Systems::TextSystem> textSys =
      Systems::SystemManager::GetInstance().GetSystem<Systems::TextSystem>().lock()) {
      if (!textSys->IsValid(*this))
        textSys->AddFont(*this);
    }
  }

  TextSystem::TextSystem(const char* name) : System(name), mFreeTypeLib{}, mFonts{}, mShader{} {
    if (FT_Init_FreeType(&mFreeTypeLib)) {
      throw Debug::Exception<TextSystem>(Debug::EXCEPTION_LEVEL::LVL_CRITICAL, Msg("FreeType could not be Initialized"));
    }

    else {
      Debug::DebugLogger::GetInstance().LogInfo("[Text] FreeType successfully Initialized");
    }

    Graphics::ShaderLibrary::Add("Font", Graphics::Shader::Create("Font.vert.glsl", "Font.frag.glsl"));

    try {
      mShader = Graphics::ShaderLibrary::Get("Font");
    } catch([[maybe_unused]] const std::out_of_range& e) {
      throw Debug::Exception<TextSystem>(Debug::EXCEPTION_LEVEL::LVL_CRITICAL, Msg("Shader Program not Found"));
    }
  }

  void TextSystem::Start() {
    SUBSCRIBE_CLASS_FUNC(Events::EventType::LOAD_SCENE, &TextSystem::OnSceneLoad, this);
  }

  void TextSystem::Update() {
    // Render the fonts for all Entities with Text Components
    // @TODO: Account for potential UI Component
    auto const& textEntities{ 
      ECS::EntityManager::GetInstance().GetAllEntitiesWithComponents<Component::Text, Component::Transform>() 
    };

    for (auto const& entity : textEntities) {
      auto const& textComp{ ECS::Entity{entity}.GetComponent<Component::Text>() };
      auto const& transComp{ ECS::Entity{entity}.GetComponent<Component::Transform>() };

      if (!IGE_ASSETMGR.IsGUIDValid<IGE::Assets::FontAsset>(textComp.textAsset)) {
        Debug::DebugLogger::GetInstance().LogWarning("[Text] Invalid Text Asset attached to Entity: "
          + ECS::Entity{entity}.GetTag());
        continue;
      }

      Systems::Font const& fontAsset{ IGE_ASSETMGR.GetAsset<IGE::Assets::FontAsset>(textComp.textAsset)->mFont };
      if (!IsValid(fontAsset)) {
        Debug::DebugLogger::GetInstance().LogWarning("[Text] Invalid Font attached to Entity: "
          + ECS::Entity{ entity }.GetTag());
        continue;
      }

      float lengthOfText{ 
        TextSystem::GetTextWidth(fontAsset.mFilePathHash, textComp.textContent, textComp.scale)
      };
      
      TextSystem::RenderText(fontAsset.mFilePathHash, textComp.textContent, 
        transComp.position.x - (lengthOfText / 2.f), transComp.position.y,
        textComp.scale, textComp.color);
    }
  }

  void TextSystem::Destroy() {
    for (std::pair<const uint32_t, std::shared_ptr<Systems::Font>>& font : mFonts) {
      FaceObject& curr = font.second->mFace;
      FT_Done_Face(curr.face);
    }

    FT_Done_FreeType(mFreeTypeLib);
  }

  void TextSystem::AddFont(Systems::Font& font) {
    Debug::DebugLogger::GetInstance().LogInfo("[Text] Adding Font " + font.mFilePath);

    // Check if the font has already been loaded
    if (mFonts.find(font.mFilePathHash) != mFonts.end()) {
      Debug::DebugLogger::GetInstance().LogInfo("[Text] Font already loaded");
      font = *mFonts[font.mFilePathHash];
      return;
    }

    // Add to font map
    mFonts[font.mFilePathHash] = std::make_shared<Systems::Font>(font);

    TextSystem::LoadFontFace(*mFonts[font.mFilePathHash]);
    TextSystem::SetFontSize(font.mFilePathHash, DEFAULT_FONT_SIZE);
    TextSystem::GenerateBitmap(font.mFilePathHash, DEFAULT_FONT_SIZE);
  }

  std::unordered_map<uint32_t, std::shared_ptr<Systems::Font>> const& TextSystem::GetLoadedFontMap() const {
    return mFonts;
  }

  void TextSystem::RenderText(uint32_t filePathHash, std::string textContent, 
    float xPos, float yPos, float scale, glm::vec3 color) {

  }

  float TextSystem::GetTextWidth(uint32_t filePathHash, std::string textContent, float scale) {
    float xPos{ 0.f };
    std::shared_ptr<Systems::Font> font{ mFonts[filePathHash] };

    for (char const& ch : textContent) {
      Character currChar{ (font->mCharacterMap)[ch] };
      xPos += (currChar.advance >> 6) * scale;
    }

    return xPos;
  }

  bool TextSystem::IsValid(Font const& font) const {
    if (font.mCharacterMap.empty() || !font.mBitmap || !font.mFace.face) return false;
    return true;
  }

  void TextSystem::LoadFontFace(Systems::Font& font) const {
    FT_Error result{ FT_New_Face(mFreeTypeLib, font.mFilePath.c_str(), 0, &font.mFace.face)};

    if (result) {
      Debug::DebugLogger::GetInstance().LogError("[Text] Failed to add font face for " + font.mFilePath);
      return;
    }
  }

  void TextSystem::SetFontSize(uint32_t filePathHash, int fontSize) {
    std::shared_ptr<Systems::Font> font{ mFonts[filePathHash] };
    FT_Error result{ FT_Set_Pixel_Sizes(font->mFace.face, 0, fontSize) };

    if (result) {
      Debug::DebugLogger::GetInstance().LogError("[Text] Failed to add font face for " + font->mFilePath);
      return;
    }
  }

  void TextSystem::GenerateBitmap(uint32_t filePathHash, int fontSize) {
    std::shared_ptr<Systems::Font> font{ mFonts[filePathHash] };

    FT_UInt glyphIndex{};
    FT_Error error{};
    FT_Face const& currFace{ font->mFace.face };

    // Size of Bitmap
    int imageWidth{ (fontSize + 6) * 16 };
    int imageHeight{ (fontSize + 2) * 8 };

    // Create a buffer for the bitmap (multiply by 4 for colors)
    unsigned char* buffer = new unsigned char[imageWidth * imageHeight * 4] {};

    // Create an array to save the character widths
    int* widths = new int[MAX_ASCII];
    int* heights = new int[MAX_ASCII];

    for (int ch{}; ch < MAX_ASCII; ++ch) {
      glyphIndex = FT_Get_Char_Index(currFace, ch);
      error = FT_Load_Glyph(currFace, glyphIndex, FT_LOAD_RENDER);

      if (error) {
        Debug::DebugLogger::GetInstance().LogInfo("[Text] Failed to load Glyph for " + font->mFilePath + 
          " with FreeType Error code: " + std::to_string(error));
        continue;
      }

      error = FT_Render_Glyph(currFace->glyph, FT_RENDER_MODE_NORMAL);
      if (error) {
        Debug::DebugLogger::GetInstance().LogInfo("[Text] Failed to render Glyph for " + font->mFilePath +
          " with FreeType Error code: " + std::to_string(error));
      }

      // Save the character information
      widths[ch] = currFace->glyph->metrics.width / 64;
      heights[ch] = currFace->glyph->metrics.height / 64;

      // Find the tile position where we have to draw the character
      int x{ (ch % 16) * (fontSize + 6) };
      int y{ (ch / 16) * (fontSize + 2) };
      x += 3; // 3 pixel padding from the left side of the tile

      // Draw the character
      const FT_Bitmap& bitmap = currFace->glyph->bitmap;
      for (unsigned int xx{}; xx < bitmap.width; ++xx) {
        for (unsigned int yy{}; yy < bitmap.rows; ++yy) {
          unsigned char r = bitmap.buffer[(yy * (bitmap.width) + xx)];
          buffer[(y + yy) * imageWidth * 4 + (x + xx) * 4 + 0] = r;
          buffer[(y + yy) * imageWidth * 4 + (x + xx) * 4 + 1] = r;
          buffer[(y + yy) * imageWidth * 4 + (x + xx) * 4 + 2] = r;
          buffer[(y + yy) * imageWidth * 4 + (x + xx) * 4 + 3] = 255;
        }
      }

      Character currChar{};

      currChar.size = { currFace->glyph->bitmap.width, currFace->glyph->bitmap.rows };
      currChar.bearing = { currFace->glyph->bitmap_left, currFace->glyph->bitmap_top };
      currChar.advance = currFace->glyph->advance.x;

      currChar.uMin = static_cast<float>(x) / imageWidth;
      currChar.vMin = static_cast<float>(y) / imageHeight;
      currChar.uMax = currChar.uMin + static_cast<float>(widths[ch]) / imageWidth;
      currChar.vMax = currChar.vMin + static_cast<float>(heights[ch]) / imageHeight;

      (font->mCharacterMap)[static_cast<char>(ch)] = currChar;
    }

    #ifdef _DEBUG
        DebugGlyph(font->mFilePathHash, imageWidth, imageHeight, buffer);
    #endif

    std::shared_ptr<Graphics::Texture> bitmapTex;
    bitmapTex.reset(new Graphics::Texture(imageWidth, imageHeight,
      static_cast<const void*>(buffer)));

    delete[] buffer;
    delete[] widths;
    delete[] heights;

    font->mBitmap = bitmapTex;
    CreateGLObjects(font->mFilePathHash);
  }

  void TextSystem::CreateGLObjects(uint32_t filePathHash) {
    std::shared_ptr<Graphics::VertexArray> currVao;
    currVao.reset(new Graphics::VertexArray);

    std::shared_ptr<Graphics::VertexBuffer> currVbo;

    // position and texture coordinates
    currVbo.reset(new Graphics::VertexBuffer(MAX_VERTICES * sizeof(float)));

    Graphics::BufferLayout fontLayout = {
      { Graphics::AttributeType::VEC2, "a_Position" },
      { Graphics::AttributeType::VEC2, "a_TexCoord" }
    };

    currVbo->SetLayout(fontLayout);
    currVao->AddVertexBuffer(currVbo);

    unsigned int* quadIndices = new unsigned int[MAX_INDICES];
    unsigned int offset{};
    for (unsigned int i{}; i < MAX_INDICES; i += 6) {
      quadIndices[i + 0] = offset + 0;
      quadIndices[i + 1] = offset + 1;
      quadIndices[i + 2] = offset + 2;

      quadIndices[i + 3] = offset + 2;
      quadIndices[i + 4] = offset + 3;
      quadIndices[i + 5] = offset + 0;

      offset += 4;
    }

    std::shared_ptr<Graphics::ElementBuffer> currEbo;
    currEbo.reset(new Graphics::ElementBuffer(quadIndices, MAX_INDICES));
    currVao->SetElementBuffer(currEbo);

    FaceObject& curr = mFonts[filePathHash]->mFace;
    curr.vao = currVao;
    curr.vbo = currVbo;
    curr.ebo = currEbo;

    delete[] quadIndices;
  }

  void TextSystem::DebugGlyph(uint32_t filePathHash, [[maybe_unused]] int width, 
    [[maybe_unused]] int height, [[maybe_unused]] unsigned char* buffer) {
    // Define the output file path (e.g., based on character's Unicode value).
    std::string outputPath = "../Glyphs/test.png";

    // Save the glyph image as a PNG file.
    int result = stbi_write_png(outputPath.c_str(), width, height, 4, buffer, 0);

    if (result) {
      Debug::DebugLogger::GetInstance().LogInfo("[Text] Image saved successfully to " + outputPath);
    }
    else {
      Debug::DebugLogger::GetInstance().LogInfo("[Text] Failed to save image to " + outputPath);
    }
  }

  EVENT_CALLBACK_DEF(TextSystem, OnSceneLoad) {
    // Load the fonts for all Entities with Text Components
    // @TODO: Account for potential UI Component
    auto const& textEntities{
      ECS::EntityManager::GetInstance().GetAllEntitiesWithComponents<Component::Text>()
    };

    for (auto const& entity : textEntities) {
      auto const& textComp{ ECS::Entity{entity}.GetComponent<Component::Text>() };

      IGE_ASSETMGR.LoadRef<IGE::Assets::FontAsset>(textComp.textAsset);
      if (!IGE_ASSETMGR.IsGUIDValid<IGE::Assets::FontAsset>(textComp.textAsset)) {
        Debug::DebugLogger::GetInstance().LogWarning("[Text] Invalid Text Asset attached to Entity: "
          + ECS::Entity{ entity }.GetTag());
        continue;
      }

      Systems::Font& fontAsset{ IGE_ASSETMGR.GetAsset<IGE::Assets::FontAsset>(textComp.textAsset)->mFont };
      if (!IsValid(fontAsset)) {
        AddFont(fontAsset);
      }
    }
  }

}