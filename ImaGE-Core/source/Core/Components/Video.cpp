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
    Video::Video(IGE::Assets::GUID _guid) : buffer{}, texture{}, videoSource{}, audioSource{}, guid{},
    renderType{ RenderType::WORLD }, started{ false }, paused{ false },
    playOnStart{ true }, loop{ false }, audioEnabled{ true }, channelGroup{ IGE::Audio::AudioManager::GetInstance().CreateGroup() } {}

    Video::Video(Video const& rhs) : buffer{}, texture{}, videoSource{}, audioSource{}, guid{ rhs.guid },
    renderType{ rhs.renderType }, started{ false }, paused{ false },
        playOnStart{ rhs.playOnStart }, loop{ rhs.loop }, audioEnabled{ rhs.audioEnabled }, audioOffset{rhs.audioOffset},
      sound{ rhs.sound }, audioPlaySettings{ rhs.audioPlaySettings }, channelGroup {
      IGE::Audio::AudioManager::GetInstance().CreateGroup()
  } {
  }

  Video& Video::operator=(Video const& rhs) {
    renderType = rhs.renderType;
    playOnStart = rhs.playOnStart;
    guid = rhs.guid;
    audioEnabled = rhs.audioEnabled;
    loop = rhs.loop;

    audioOffset = rhs.audioOffset;
    sound = rhs.sound;
    audioPlaySettings = rhs.audioPlaySettings;
    
    return *this;
  }

  void Video::InitVideoSource(IGE::Assets::GUID _guid) {
    Release();  // clear previous video if needed

    std::string const& path{ 
      IGE_ASSETMGR.GetAsset<IGE::Assets::VideoAsset>(
        IGE_ASSETMGR.LoadRef<IGE::Assets::VideoAsset>(_guid)
      )->mVideoPath
    };

    videoSource = plm_create_with_filename(path.c_str());
    audioSource = plm_create_with_filename(path.c_str());

    if (!videoSource || !audioSource) {
      throw Debug::Exception<Video>(Debug::LVL_ERROR, 
        Msg("Unable to create video buffer for " + path));
    }
    plm_set_video_enabled(audioSource, false);
    plm_set_audio_enabled(videoSource, false);

    double leadTime = plm_get_duration(videoSource) * 0.2; // 10% of video
    int sampleRate = plm_get_samplerate(videoSource);
    int channels = 2; //assuming 2
    
    sound = IGE::Audio::Sound{ sampleRate, channels };
    auto soundptr{ sound.GetSoundPtr() };
    soundptr->setUserData(this);
    plm_set_video_decode_callback(videoSource, VideoDecodeCallback, this);
    plm_set_audio_decode_callback(audioSource, AudioDecodeCallback, this);

    // init texture
    texture = std::make_unique<Graphics::Texture>(
      static_cast<uint32_t>(plm_get_width(videoSource)),
      static_cast<uint32_t>(plm_get_height(videoSource)),
      GL_RGB8 // not sure if we need alpha channel
    );
    buffer.resize(texture->GetWidth() * texture->GetHeight() * 3);
    
    // set the image to the first frame as a preview
    bool const originalAudioEnabled{ audioEnabled };
    //if (originalAudioEnabled) {
    //  EnableAudio(false); // we don't require audio for the previw
    //}

    if (!PreviewFirstFrame()) {
      throw Debug::Exception<Video>(Debug::LVL_ERROR, Msg("Unable to decode video frame from " + path));
    }

    EnableAudio(originalAudioEnabled);
    SetLoop(loop);

    //preload audio
    plm_decode(audioSource, leadTime);

    guid = _guid;
  }

  float Video::GetVideoTimestamp() const {
    return static_cast<float>(plm_get_time(videoSource));
  }

  float Video::GetVideoLength() const {
    return static_cast<float>(plm_get_duration(videoSource));
  }

  bool Video::HasVideoEnded() const {
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

  void Video::EnableAudio(bool enabled) {
    plm_set_audio_enabled(audioSource, enabled);
    audioEnabled = enabled;
  }

  bool Video::IsLoopEnabled() const {
    return static_cast<bool>(plm_get_loop(videoSource));
  }

  void Video::SetLoop(bool enabled) {
    plm_set_loop(videoSource, enabled);
    plm_set_loop(audioSource, enabled);
    loop = enabled;
  }

  bool Video::PreviewFirstFrame() {
    plm_frame_t* frame{ plm_decode_video(videoSource) };
    if (!frame) { return false; }

    plm_frame_to_rgb(frame, buffer.data(), frame->width * 3);

    GLCALL(glTextureSubImage2D(
      texture->GetTexHdl(),
      0, 0, 0,
      frame->width, frame->height,
      GL_RGB, GL_UNSIGNED_BYTE, buffer.data())
    );

    plm_rewind(videoSource); // reset it back to default
    return true;
  }

  void Video::Release() {
    buffer.clear();
    texture.reset();
    if (videoSource) {
      plm_destroy(videoSource);
      videoSource = nullptr;
    }
    if (audioSource) {
        plm_destroy(audioSource);
        audioSource = nullptr;
    }
  }

  void Video::Clear() noexcept {
    Release();

    renderType = RenderType::WORLD;
    guid = {};
    playOnStart = audioEnabled = true;
    started = paused = loop = false;
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
    plm_frame_to_rgb(frame, video->buffer.data(), frame->width * 3);

    // update the texture buffer
    GLCALL(glTextureSubImage2D(
      video->texture->GetTexHdl(),
      0, 0, 0,
      frame->width, frame->height,
      GL_RGB, GL_UNSIGNED_BYTE, video->buffer.data())
    );
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
