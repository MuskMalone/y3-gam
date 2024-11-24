#include <pch.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

#include "TextSystem.h"
#include "Core/GUID.h"
#include "Core/Components/Text.h"
#include "Core/Components/Transform.h"
#include "Core/Components/Canvas.h"

namespace Systems {

  Font::Font(std::string const& filePath) : mFilePath{ filePath }, 
    mFilePathHash{ IGE::Core::Fnv1a32(filePath.c_str(), filePath.size()) }, mMaxHeight{} {
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

  }

  void TextSystem::Destroy() {
    for (std::pair<const uint32_t, std::shared_ptr<Systems::Font>>& font : mFonts) {
      FaceObject& curr = font.second->mFace;
      FT_Done_Face(curr.face);
    }

    FT_Done_FreeType(mFreeTypeLib);
  }

  void TextSystem::AddFont(Systems::Font& font) {
    Debug::DebugLogger::GetInstance().LogInfo("[Text] Adding Font \"" + font.mFilePath + "\"");

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
    font = *mFonts[font.mFilePathHash];
  }

  std::unordered_map<uint32_t, std::shared_ptr<Systems::Font>> const& TextSystem::GetLoadedFontMap() const {
    return mFonts;
  }

  void TextSystem::RenderTextForAllEntities(glm::mat4 viewProj, std::vector<ECS::Entity> const& entities) {
    for (auto const& entity : entities) {
      if (entity.HasComponent<Component::Text, Component::Transform>()) {
        auto& textComp{ ECS::Entity{entity}.GetComponent<Component::Text>() };
        auto const& transComp{ ECS::Entity{entity}.GetComponent<Component::Transform>() };

        if (!IGE_ASSETMGR.IsGUIDValid<IGE::Assets::FontAsset>(textComp.textAsset)) {
          Debug::DebugLogger::GetInstance().LogWarning("[Text] Invalid Text Asset attached to Entity: "
            + ECS::Entity{ entity }.GetTag());
          continue;
        }

        Systems::Font const& fontAsset{ IGE_ASSETMGR.GetAsset<IGE::Assets::FontAsset>(textComp.textAsset)->mFont };
        if (!IsValid(fontAsset)) {
          Debug::DebugLogger::GetInstance().LogWarning("[Text] Invalid Font attached to Entity: "
            + ECS::Entity{ entity }.GetTag());
          continue;
        }

        TextSystem::CalculateNewLineIndices(textComp.textAsset, textComp.textContent,
          textComp.newLineIndices, textComp.newLineIndicesUpdatedFlag, textComp.alignment);

        TextSystem::RenderText(fontAsset.mFilePathHash, textComp.textContent,
          transComp.position.x, transComp.position.y, textComp.scale, transComp.rotation,
          textComp.color, textComp.newLineIndices, textComp.multiLineSpacingOffset, viewProj);
      }
    }
  }

/*
  void TextSystem::DrawTextFont(std::shared_ptr<Graphics::Shader> const& shader, uint32_t filePathHash, 
    std::string const& textContent, float xPos, float yPos, float scale, glm::quat rotation,
    glm::vec4 color, std::vector<std::pair<size_t, float>> const& newLineIndices, int multiLineSpacingOffset) {
    if (mFonts.find(filePathHash) == mFonts.end()) {
      Debug::DebugLogger::GetInstance().LogWarning("[Text] Trying to Render Invalid Font");
      return;
    }

    std::shared_ptr<Systems::Font> font{ mFonts[filePathHash] };

    yPos = -yPos;
    FaceObject const& currFace{ font->mFace };
    std::shared_ptr<Graphics::Texture>const& currTex{ font->mBitmap };

    currFace.vao->Bind();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);

    // update VBO for each character
    GLfloat const initialXPos = xPos;
    size_t currLineIdx{};
    size_t currNewLineIndicesVectorIdx{};

    xPos += scale * newLineIndices[currNewLineIndicesVectorIdx].second;
    ++currNewLineIndicesVectorIdx;

    std::vector<float> vertices{};
    std::size_t verticesPerCharacter{ static_cast<size_t>(4 * 10) };
    vertices.reserve(verticesPerCharacter * textContent.length());
    vertices.clear();
    int index{};

    for (size_t currStringIdx{}; currStringIdx < textContent.size(); ++currStringIdx) {
      char ch = textContent[currStringIdx];
      Character const& currChar{ (font->mCharacterMap)[ch] };

      if (currNewLineIndicesVectorIdx < newLineIndices.size()
        && currStringIdx == (newLineIndices[currNewLineIndicesVectorIdx].first - 1)
        && currLineIdx != newLineIndices[currNewLineIndicesVectorIdx].first) {
        yPos += (multiLineSpacingOffset + font->mMaxHeight * scale);
        xPos = initialXPos + scale * newLineIndices[currNewLineIndicesVectorIdx].second;

        currLineIdx = newLineIndices[currNewLineIndicesVectorIdx].first;
        ++currNewLineIndicesVectorIdx;
        continue; // skip rendering this character (as it is the new line character)
      }

      float xpos{ xPos + currChar.bearing.x * scale };
      float ypos{ yPos - (currChar.size.y + currChar.bearing.y) * scale };

      float width{ currChar.size.x * scale };
      float height{ currChar.size.y * scale };
      
      float texIdx = 0; // White Tex

      float arr[4 * 10] = {
        // Vertex 1: position (xpos + width, ypos + height, 0.0), color, texCoord, texIdx
        xpos + width, ypos + height, 0.0f, color.r, color.g, color.b, color.a, currChar.uMax, currChar.vMin, texIdx,
        // Vertex 2: position (xpos + width, ypos + 2.f * height, 0.0), color, texCoord, texIdx
        xpos + width, ypos + 2.f * height, 0.0f, color.r, color.g, color.b, color.a, currChar.uMax, currChar.vMax, texIdx,
        // Vertex 3: position (xpos, ypos + 2.f * height, 0.0), color, texCoord, texIdx
        xpos, ypos + 2.f * height, 0.0f, color.r, color.g, color.b, color.a, currChar.uMin, currChar.vMax, texIdx,
        // Vertex 4: position (xpos, ypos + height, 0.0), color, texCoord, texIdx
        xpos, ypos + height, 0.0f, color.r, color.g, color.b, color.a, currChar.uMin, currChar.vMin, texIdx
      };

      auto insertPosition{ vertices.begin() + (index * verticesPerCharacter) };
      vertices.insert(insertPosition, arr, arr + std::size(arr));

      xPos += (currChar.advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
      index++;
    }

    // bind the bitmap font texture
    currTex->Bind();

    currFace.vbo->Bind();
    currFace.vbo->SetData(vertices.data(),
      static_cast<unsigned int>(vertices.size() * sizeof(float)));
    currFace.ebo->Bind();
    size_t eboSizePerChar{ 6 };

    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(eboSizePerChar* textContent.length()),
      GL_UNSIGNED_INT, NULL);

    glEnable(GL_DEPTH_TEST);
    currFace.vao->Unbind();
    currFace.ebo->Unbind();
    currFace.vbo->Unbind();
    //shader->Unuse();
  }
  */

  float TextSystem::GetTextWidth(uint32_t filePathHash, std::string const& textContent, float scale) {
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

  void TextSystem::CalculateNewLineIndices(IGE::Assets::GUID textAsset, std::string const& textContent, 
    std::vector<std::pair<size_t, float>>& indices, bool& newLineIndicesUpdatedFlag, int alignment) {
    if (newLineIndicesUpdatedFlag) {
      return;
    }

    IGE_ASSETMGR.LoadRef<IGE::Assets::FontAsset>(textAsset);
    if (!IGE_ASSETMGR.IsGUIDValid<IGE::Assets::FontAsset>(textAsset)) {
      Debug::DebugLogger::GetInstance().LogWarning("[Text] Invalid Text Asset");
      return;
    }

    Systems::Font& fontAsset{ IGE_ASSETMGR.GetAsset<IGE::Assets::FontAsset>(textAsset)->mFont };

    if (mFonts.find(fontAsset.mFilePathHash) == mFonts.end()) {
      Debug::DebugLogger::GetInstance().LogWarning("[Text] Trying to Calculate New Lines for Invalid Font");
      return;
    }

    size_t currIdx{};
    indices.clear();
    indices.emplace_back(std::pair<size_t, float>(currIdx++, 0));
    std::shared_ptr<Systems::Font> font{ mFonts[fontAsset.mFilePathHash] };

    for (auto ch : textContent) {
      if (ch == '\n') {
        indices.emplace_back(std::pair<size_t, float>(currIdx, 0));
        ++currIdx;
        continue;
      }

      float const width{ static_cast<float>(font->mCharacterMap[ch].advance >> 6)};

      switch (alignment) {
      case Component::Text::Alignment::LEFT:
        break;

      case Component::Text::Alignment::RIGHT:
        indices.back().second -= width;
        break;

      case Component::Text::Alignment::CENTER:
        indices.back().second -= width * 0.5f;
        break;
      
      default:
        Debug::DebugLogger::GetInstance().LogWarning("[Text] Invalid Text Alignment");
        break;
      }

      ++currIdx;
    }

    newLineIndicesUpdatedFlag = true;
  }

  void TextSystem::RenderText(uint32_t filePathHash, std::string const& textContent,
    float xPos, float yPos, float scale, glm::quat rotation, glm::vec3 color,
    std::vector<std::pair<size_t, float>> const& newLineIndices, float multiLineSpacingOffset, glm::mat4 viewProj) {
    if (mFonts.find(filePathHash) == mFonts.end()) {
      Debug::DebugLogger::GetInstance().LogWarning("[Text] Trying to Render Invalid Font");
      return;
    }

    std::shared_ptr<Systems::Font> font{ mFonts[filePathHash] };
    FaceObject const& currFace{ font->mFace };
    std::shared_ptr<Graphics::Texture>const& currTex{ font->mBitmap };

    glm::mat4 projection{ viewProj };

    yPos = -yPos;
    glm::mat4 flipY{ glm::mat4(1.0f) };
    flipY[1][1] = -1.0f;
    projection = projection * flipY;

    // Calculate total text block height (For centering the Y of the text even if multi line)
    float totalHeight = font->mMaxHeight * scale; // First line height
    if (!newLineIndices.empty()) {
      totalHeight += (newLineIndices.size() - 1) * (multiLineSpacingOffset + font->mMaxHeight * scale);
    }

    // Adjust initial yPos to vertically center the text block
    yPos -= totalHeight / 2.0f;

    // update VBO for each character
    GLfloat const initialXPos = xPos;
    size_t currLineIdx{};
    size_t currNewLineIndicesVectorIdx{};

    xPos += scale * newLineIndices[currNewLineIndicesVectorIdx].second;
    ++currNewLineIndicesVectorIdx;

    std::vector<float> vertices{};
    std::size_t verticesPerCharacter{ static_cast<size_t>(4 * 4) };
    vertices.reserve(verticesPerCharacter * (textContent.length() - newLineIndices.size() + 1));
    vertices.clear();
    int index{};

    for (size_t currStringIdx{}; currStringIdx < textContent.size(); ++currStringIdx) {
      char ch = textContent[currStringIdx];
      Character const& currChar{ (font->mCharacterMap)[ch] };

      if (currNewLineIndicesVectorIdx < newLineIndices.size()
        && currStringIdx == (newLineIndices[currNewLineIndicesVectorIdx].first - 1)
        && currLineIdx != newLineIndices[currNewLineIndicesVectorIdx].first) {
        yPos += (multiLineSpacingOffset + font->mMaxHeight * scale);
        xPos = initialXPos + scale * newLineIndices[currNewLineIndicesVectorIdx].second;

        currLineIdx = newLineIndices[currNewLineIndicesVectorIdx].first;
        ++currNewLineIndicesVectorIdx;
        continue; // skip rendering this character (as it is the new line character)
      }

      float xpos{ xPos + currChar.bearing.x * scale };
      float ypos{ yPos - (currChar.size.y + currChar.bearing.y) * scale };

      float width{ currChar.size.x * scale };
      float height{ currChar.size.y * scale };

      glm::vec4 topRight(xpos + width, ypos + height, 0.0f, 1.0f);
      glm::vec4 bottomRight(xpos + width, ypos + 2.f * height, 0.0f, 1.0f);
      glm::vec4 bottomLeft(xpos, ypos + 2.f * height, 0.0f, 1.0f);
      glm::vec4 topLeft(xpos, ypos + height, 0.0f, 1.0f);

      // Store the transformed positions in arr[4*4]
      float arr[4 * 4] = {
          topRight.x, topRight.y, currChar.uMax, currChar.vMin,
          bottomRight.x, bottomRight.y, currChar.uMax, currChar.vMax,
          bottomLeft.x, bottomLeft.y, currChar.uMin, currChar.vMax,
          topLeft.x, topLeft.y, currChar.uMin, currChar.vMin
      };

      auto insertPosition{ vertices.begin() + (index * verticesPerCharacter) };
      vertices.insert(insertPosition, arr, arr + std::size(arr));

      xPos += (currChar.advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
      index++;
    }

    // bind the bitmap font texture
    currFace.vbo->SetData(nullptr, 0);
    currTex->Bind(0);

    mShader->Use();
    mShader->SetUniform("uTextColor", color.x, color.y, color.z);
    mShader->SetUniform("uProjection", projection);
    currFace.vao->Bind();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);
    glDepthFunc(GL_ALWAYS);
    glDisable(GL_CULL_FACE);

    currFace.vbo->Bind();
    currFace.vbo->SetData(vertices.data(),
      static_cast<unsigned int>(vertices.size() * sizeof(float)));
    currFace.ebo->Bind();
    size_t eboSizePerChar{ 6 };

    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(eboSizePerChar * (textContent.length() - newLineIndices.size() + 1)),
      GL_UNSIGNED_INT, NULL);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    currFace.vao->Unbind();
    currFace.ebo->Unbind();
    currFace.vbo->Unbind();
    mShader->Unuse();
  }

  void TextSystem::LoadFontFace(Systems::Font& font) const {
    FT_Error result{ FT_New_Face(mFreeTypeLib, font.mFilePath.c_str(), 0, &font.mFace.face)};

    if (result) {
      Debug::DebugLogger::GetInstance().LogError("[Text] Failed to add font face for \"" + font.mFilePath + "\"");
      return;
    }
  }

  void TextSystem::SetFontSize(uint32_t filePathHash, int fontSize) {
    std::shared_ptr<Systems::Font> font{ mFonts[filePathHash] };
    FT_Error result{ FT_Set_Pixel_Sizes(font->mFace.face, 0, fontSize) };

    if (result) {
      Debug::DebugLogger::GetInstance().LogError("[Text] Failed to add font face for \"" + font->mFilePath + "\"");
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

    float maxFontHeight{};

    for (int ch{}; ch < MAX_ASCII; ++ch) {
      glyphIndex = FT_Get_Char_Index(currFace, ch);
      error = FT_Load_Glyph(currFace, glyphIndex, FT_LOAD_RENDER);

      if (error) {
        Debug::DebugLogger::GetInstance().LogInfo("[Text] Failed to load Glyph for \"" + font->mFilePath + 
          "\" with FreeType Error code: " + std::to_string(error));
        continue;
      }

      error = FT_Render_Glyph(currFace->glyph, FT_RENDER_MODE_NORMAL);
      if (error) {
        Debug::DebugLogger::GetInstance().LogInfo("[Text] Failed to render Glyph for \"" + font->mFilePath +
          "\" with FreeType Error code: " + std::to_string(error));
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

      if (ch > 32 && ch < 127) {
        float currHeight = static_cast<float>(currFace->glyph->bitmap_top);
        maxFontHeight = (currHeight > maxFontHeight) ? currHeight : maxFontHeight;
      }
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

    font->mMaxHeight = maxFontHeight;
    font->mBitmap = bitmapTex;
    CreateGLObjects(font->mFilePathHash);
  }

  void TextSystem::CreateGLObjects(uint32_t filePathHash) {
    std::shared_ptr<Graphics::VertexArray> currVao;
    currVao.reset(new Graphics::VertexArray);

    std::shared_ptr<Graphics::VertexBuffer> currVbo;

    // position and texture coordinates
    currVbo.reset(new Graphics::VertexBuffer(MAX_VERTICES * sizeof(float)));

    /*
    Graphics::BufferLayout fontLayout = {
      { Graphics::AttributeType::VEC3, "a_Position" },
      { Graphics::AttributeType::VEC4, "a_Color" },
      { Graphics::AttributeType::VEC2, "a_TexCoord" },
      { Graphics::AttributeType::FLOAT, "a_TexIdx" }
    };
    */

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
    // Define the output file path
    std::string outputPath = "../Assets/Fonts/Glyphs/" + std::to_string(filePathHash) + ".png";

    // Skip if the glyph has already been generated
    if (std::filesystem::exists(outputPath)) {
      return;
    }

    int result = stbi_write_png(outputPath.c_str(), width, height, 4, buffer, 0);

    if (result) {
      Debug::DebugLogger::GetInstance().LogInfo("[Text] Image saved successfully to \"" + outputPath + "\"");
    }
    else {
      Debug::DebugLogger::GetInstance().LogInfo("[Text] Failed to save image to \"" + outputPath + "\"");
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