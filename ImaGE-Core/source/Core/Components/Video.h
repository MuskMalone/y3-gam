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

    void Init();
    void PlayVideo();
    void TogglePause() noexcept;  // note: only works for video

    float GetVideoTimestamp() const;
    float GetVideoLength() const;
    bool HasVideoEnded() const;
    float GetFramerate() const;
    float GetAspectRatio() const;

    inline bool IsAudioEnabled() const noexcept { return audioEnabled; }
    void EnableAudio(bool enabled);

    bool IsLoopEnabled() const noexcept { return loop; }
    void SetLoop(bool enabled);

    bool IsPlayedOnAwake() const noexcept { return playOnStart; }
    void SetPlayOnAwake(bool playOnAwake);

    void AdvanceVideo(float seconds);
    void SetAlpha(unsigned newAlpha);

    inline bool IsWorldObject() const noexcept{ return renderType == RenderType::WORLD; }
    inline bool IsUIObject() const noexcept { return renderType == RenderType::UI; }

    void UpdateBuffer();
    void ClearFrame();  // fills the texture with black
    void Release();
    void Clear() noexcept;

  private:
    void InitVideoSource(IGE::Assets::GUID guid);
    void InitAudioSource(IGE::Assets::GUID guid);
    bool PreviewFirstFrame();
    void FreeSources();

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
    bool playOnStart;   // don't modify this directly!    // the variables at this point
    bool loop;          // don't modify this directly!
    bool audioEnabled;  // don't modify this directly!
  };
}
