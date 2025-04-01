#pragma once
#include <Asset/SmartPointer.h>
#include <memory>
#include <Graphics/Texture.h>
#include "Audio/AudioManager.h"

struct plm_t;

namespace Component {
  struct Video {
    Video();
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

    inline bool IsAudioEnabled() const noexcept { return audioEnabled; }
    void EnableAudio(bool enabled);

    bool IsLoopEnabled() const;
    void SetLoop(bool enabled);

    void InitVideoSource(IGE::Assets::GUID guid);
    void AdvanceVideo(float seconds);
    void SetAlpha(unsigned newAlpha);

    inline bool IsWorldObject() const noexcept{ return renderType == RenderType::WORLD; }
    inline bool IsUIObject() const noexcept { return renderType == RenderType::UI; }

    void Release();
    void Clear() noexcept;

  private:
    bool PreviewFirstFrame();

  public:
    enum RenderType {
      WORLD = 0,
      UI,
      NUM_TYPES
    };

    IGE::Audio::SoundInvokeSetting audioPlaySettings;
    IGE::Audio::Sound sound;
    std::vector<uint8_t> buffer;
    std::unique_ptr<Graphics::Texture> texture;
    uint64_t channelGroup{};
    plm_t* videoSource;
    plm_t* audioSource;

    IGE::Assets::GUID guid;
    RenderType renderType;
    unsigned alpha;     // don't modify this directly!
    unsigned audioOffset;
    bool started, paused;                                 // should probably just private
    bool playOnStart;                                     // the variables at this point
    bool loop;          // don't modify this directly!
    bool audioEnabled;  // don't modify this directly!
  };
}
