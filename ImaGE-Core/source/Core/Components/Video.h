#pragma once
#include <Asset/SmartPointer.h>
#include <memory>
#include <Graphics/Texture.h>

struct plm_t;

namespace Component {
  struct Video {
    Video() : buffer{}, texture{}, videoSource{}, guid{}, renderType{ RenderType::WORLD },
      started{ false }, paused{ false }, playOnStart{ true } {}
    Video(IGE::Assets::GUID guid);
    Video(Video const& rhs);
    ~Video();

    // for EmplaceOrReplaceComponent
    Video& operator=(Video const& rhs);

    inline void PlayVideo() noexcept { started = true; }
    inline void TogglePause() noexcept { paused = !paused; }

    float GetVideoTimestamp() const;
    float GetVideoLength() const;
    bool HasVideoEnded() const;
    float GetFramerate() const;
    float GetAspectRatio() const;

    bool IsAudioEnabled() const;
    void EnableAudio(bool enabled);

    bool IsLoopEnabled() const;
    void SetLoop(bool enabled);

    void InitVideoSource(IGE::Assets::GUID guid);
    void AdvanceVideo(float seconds);

    inline bool IsWorldObject() const noexcept{ return renderType == RenderType::WORLD; }
    inline bool IsUIObject() const noexcept { return renderType == RenderType::UI; }

    void Clear() noexcept;
    

    enum RenderType {
      WORLD = 0,
      UI,
      NUM_TYPES
    };

    std::vector<uint8_t> buffer;
    std::unique_ptr<Graphics::Texture> texture;
    plm_t* videoSource;

    IGE::Assets::GUID guid;
    RenderType renderType;
    //float prevTimestamp, timeElapsed;
    bool started, paused;
    bool playOnStart;
  };
}
