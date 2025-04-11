#include <pch.h>
#include "Video.h"
#include <Asset/AssetManager.h>
#include <Asset/Assetables/Video/VideoAsset.h>
#include <Graphics/Utils.h>

#define PL_MPEG_IMPLEMENTATION
#include <pl_mpeg.h>

namespace {
  void VideoDecodeCallback(plm_t* self, plm_frame_t* frame, void* user);
  void AudioDecodeCallback(plm_t* self, plm_samples_t* samples, void* user);
}

namespace Component {
  Video::Video() : audioPlaySettings{}, sound{}, buffer{}, texture{},
    channelGroup{ IGE::Audio::AudioManager::GetInstance().CreateGroup() },
    videoSource{}, audioSource{}, guid{}, renderType{ RenderType::WORLD },
    alpha{ 255u }, audioOffset{ 5000u }, started{ false }, paused{ false },
    playOnStart{ true }, loop{ false }, audioEnabled{ true } {}

  Video::Video(IGE::Assets::GUID _guid) : audioPlaySettings{}, sound{}, buffer{}, texture{},
    channelGroup{ IGE::Audio::AudioManager::GetInstance().CreateGroup() },
    videoSource{}, audioSource{}, guid{}, renderType{ RenderType::WORLD }, 
    alpha{ 255u }, audioOffset{ 5000u }, started{ false }, paused{ false },
    playOnStart{ true }, loop{ false }, audioEnabled{ true } {}

  Video::Video(Video const& rhs) : audioPlaySettings{ rhs.audioPlaySettings }, sound{ rhs.sound },
    buffer{}, texture{}, channelGroup{ IGE::Audio::AudioManager::GetInstance().CreateGroup() },
    videoSource{}, audioSource{}, guid{ rhs.guid }, renderType{ rhs.renderType }, alpha{ rhs.alpha },
    audioOffset{ rhs.audioOffset }, started{ false }, paused{ false },
    playOnStart{ rhs.playOnStart }, loop{ rhs.loop }, audioEnabled{ rhs.audioEnabled }
  {
    SetPlayOnAwake(playOnStart);
  }

  Video& Video::operator=(Video const& rhs) {
    renderType = rhs.renderType;
    SetPlayOnAwake(rhs.playOnStart);
    guid = rhs.guid;
    audioEnabled = rhs.audioEnabled;
    loop = rhs.loop;
    alpha = rhs.alpha;

    audioOffset = rhs.audioOffset;
    sound = rhs.sound;
    audioPlaySettings = rhs.audioPlaySettings;
    
    return *this;
  }

  void Video::Init() {
    Release();  // clear previous video if needed

    if (!guid) { 
      IGE_DBGLOGGER.LogError("[Video] Component does not hold a video source!");
      return;
    }

    InitVideoSource(guid);
    PreviewFirstFrame();
  }

  void Video::PlayVideo() {
    started = audioPlaySettings.playOnAwake = true;
    
    InitVideoSource(guid);
    InitAudioSource(guid);

    if (!playOnStart) {
      auto& mgr = IGE::Audio::AudioManager::GetInstance();
      mgr.PlaySound(sound, audioPlaySettings, channelGroup, "video");
      auto videoSoundGrp = mgr.GetGroup(channelGroup);
      if (videoSoundGrp != nullptr) {
        int numChannels = 0;
        videoSoundGrp->getNumChannels(&numChannels);
        for (int i = 0; i < numChannels; ++i) {
          FMOD::Channel* channel = nullptr;
          videoSoundGrp->getChannel(i, &channel);
          if (channel) {
            unsigned int pos = audioOffset; // jump 5 seconds into the playback
            FMOD_RESULT result = channel->setPosition(pos, FMOD_TIMEUNIT_MS);
          }
        }
      }
    }
    SetLoop(loop);
  }

  void Video::TogglePause() noexcept {
    paused = !paused;
    audioPlaySettings.paused = paused;
    
  }

  void Video::InitVideoSource(IGE::Assets::GUID _guid) {
    std::string const& path{
      IGE_ASSETMGR.GetAsset<IGE::Assets::VideoAsset>(
        IGE_ASSETMGR.LoadRef<IGE::Assets::VideoAsset>(guid)
      )->mVideoPath
    };

    videoSource = plm_create_with_filename(path.c_str());

    if (!videoSource) {
      throw Debug::Exception<Video>(Debug::LVL_ERROR,
        Msg("Unable to create video buffer for " + path));
    }
    plm_set_audio_enabled(videoSource, false);
    plm_set_video_enabled(videoSource, true);
    plm_set_video_decode_callback(videoSource, VideoDecodeCallback, this);

    // init texture
    texture = std::make_unique<Graphics::Texture>(
      static_cast<uint32_t>(plm_get_width(videoSource)),
      static_cast<uint32_t>(plm_get_height(videoSource)),
      GL_RGBA8 // RGBA to ensure any resolution is aligned
    );
    buffer.resize(texture->GetWidth() * texture->GetHeight() * 4, alpha);
  }

  void Video::InitAudioSource(IGE::Assets::GUID _guid) {
    std::string const& path{
      IGE_ASSETMGR.GetAsset<IGE::Assets::VideoAsset>(
        IGE_ASSETMGR.LoadRef<IGE::Assets::VideoAsset>(guid)
      )->mVideoPath
    };

    audioSource = plm_create_with_filename(path.c_str());

    if (!audioSource) {
      throw Debug::Exception<Video>(Debug::LVL_ERROR,
        Msg("Unable to create audio buffer for " + path));
    }
    plm_set_video_enabled(audioSource, false);
    EnableAudio(audioEnabled);

    double leadTime = plm_get_duration(videoSource) * 0.2; // 10% of video
    int sampleRate = plm_get_samplerate(videoSource);
    int channels = 2; //assuming 2

    sound = IGE::Audio::Sound{ sampleRate, channels };
    auto soundptr{ sound.GetSoundPtr() };
    soundptr->setUserData(this);
    plm_set_audio_decode_callback(audioSource, AudioDecodeCallback, this);

    // preload audio
    plm_decode(audioSource, leadTime);
  }

  float Video::GetVideoTimestamp() const {
    return static_cast<float>(plm_get_time(videoSource));
  }

  float Video::GetVideoLength() const {
    return static_cast<float>(plm_get_duration(videoSource));
  }

  bool Video::HasVideoEnded() const {
    if (!started) { return false; }

    return static_cast<bool>(plm_has_ended(videoSource));
  }

  float Video::GetFramerate() const {
    return static_cast<float>(plm_get_framerate(videoSource));
  }

  float Video::GetAspectRatio() const {
    return static_cast<float>(plm_get_pixel_aspect_ratio(videoSource));
  }

  void Video::AdvanceVideo(float seconds) {
    plm_decode(videoSource, seconds);
    plm_decode(audioSource, seconds);
  }

  void Video::SetAlpha(unsigned newAlpha) {
    for (unsigned i{ 3 }; i < buffer.size(); i += 4) {
      buffer[i] = newAlpha;
    }

    if (!started) {
      PreviewFirstFrame();
    }

    alpha = newAlpha;
  }

  void Video::SetVolume(float newVolume) {
    glm::clamp(newVolume, 0.f, 1.f);
    audioPlaySettings.volume = newVolume;
  }

  void Video::EnableAudio(bool enabled) {
    audioEnabled = enabled;

    if (started) {
      plm_set_audio_enabled(audioSource, enabled);
    }
  }

  void Video::SetLoop(bool enabled) {
    loop = enabled;

    if (started) {
      plm_set_loop(videoSource, enabled);
      plm_set_loop(audioSource, enabled);
    }
  }

  void Video::SetPlayOnAwake(bool playOnAwake) {
    playOnStart = playOnAwake;
  }

  bool Video::PreviewFirstFrame() {
    plm_frame_t* frame{ plm_decode_video(videoSource) };
    if (!frame) { return false; }

    plm_frame_to_rgba(frame, buffer.data(), frame->width * 4);

    UpdateBuffer();

    FreeSources();
    return true;
  }

  void Video::ClearFrame() {
    constexpr uint32_t mask{ 0xFF000000 };

    // leave the alpha untouched
    for (size_t i = 0; i + 3 < buffer.size(); i += 4) {
      uint32_t* p{ reinterpret_cast<uint32_t*>(buffer.data() + i) };
      *p &= mask;
    }
    
    UpdateBuffer();
  }

  void Video::UpdateBuffer() {
    GLCALL(glTextureSubImage2D(
      texture->GetTexHdl(),
      0, 0, 0,
      texture->GetWidth(), texture->GetHeight(),
      GL_RGBA, GL_UNSIGNED_BYTE, buffer.data())
    );
  }

  void Video::FreeSources() {
    if (videoSource) {
      plm_destroy(videoSource);
      videoSource = nullptr;
    }
    if (audioSource) {
      plm_destroy(audioSource);
      audioSource = nullptr;
    }
  }

  void Video::Release() {
    buffer.clear();
    texture.reset();
    FreeSources();
  }

  void Video::Clear() noexcept {
    Release();

    *this = {};
  }

  Video::~Video() {
    if (videoSource) {
      plm_destroy(videoSource);
    }
    if (audioSource) {
      plm_destroy(audioSource);
    }
  }
} // namespace Component

namespace {
  void VideoDecodeCallback([[maybe_unused]] plm_t* self, plm_frame_t* frame, void* user) {
    if (!frame) {
      IGE_DBGLOGGER.LogError("[Video] Unable to decode video frame");
      return;
    }

    Component::Video* video{ reinterpret_cast<Component::Video*>(user) };

    //// if the video didn't advance, simply render the same frame
    //if (video->GetVideoTimestamp() == video->prevTimestamp) {
    //  return;
    //}

    //IGE_DBGLOGGER.LogInfo("Frame decoded! Timestamp: " + std::to_string(video->GetVideoTimestamp()));

    // convert to rgb format

    plm_frame_to_rgba(frame, video->buffer.data(), frame->width * 4);

    // update the texture buffer
    video->UpdateBuffer();
  }

  void AudioDecodeCallback([[maybe_unused]] plm_t* self, plm_samples_t* samples, void* user) {
    if (!samples) {
      IGE_DBGLOGGER.LogError("[Video] Unable to decode audio sample");
      return;
    }

    Component::Video* video{ reinterpret_cast<Component::Video*>(user) };
    unsigned numFloats = samples->count * 2;
    video->sound.mPCMBuffer->write(samples->interleaved, numFloats);
    //printf("READ audio has smth %u\n", numFloats);

    /* ======= SAMPLE EXAMPLE FOR SDL =======
    void app_on_audio(plm_t * mpeg, plm_samples_t * samples, void* user) {
        app_t* self = (app_t*)user;

        // Hand the decoded samples over to SDL
        int size = sizeof(float) * samples->count * 2;
        SDL_QueueAudio(self->audio_device, samples->interleaved, size);
    }
    */

  }
}
