#include <pch.h>
#include "TextSystem.h"
#include "Core/GUID.h"

namespace Systems {

  Font::Font(std::string const& filePath) : mFilePath{ filePath }, 
    mFilePathHash{ IGE::Core::Fnv1a32(filePath.c_str(), filePath.size()) } {
    if (std::shared_ptr<Systems::TextSystem> textSys =
      Systems::SystemManager::GetInstance().GetSystem<Systems::TextSystem>().lock()) {
      textSys->AddFont(mFilePathHash);
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

  }

  void TextSystem::Destroy() {
    for (std::pair<const uint32_t, Systems::Font>& font : mFonts) {
      FaceObject& curr = font.second.mFace;
      FT_Done_Face(curr.face);
    }

    FT_Done_FreeType(mFreeTypeLib);
  }

  void TextSystem::AddFont(uint32_t filePathHash) {

  }

  EVENT_CALLBACK_DEF(TextSystem, OnSceneLoad) {
    // Load the fonts for all Entities with Text Components

  }

}