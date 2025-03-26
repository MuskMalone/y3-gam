#include <pch.h>
#include "Video.h"
#include <Asset/AssetManager.h>
#include <Asset/Assetables/Video/VideoAsset.h>
#include <Graphics/Utils.h>

#define PL_MPEG_IMPLEMENTATION
#include <pl_mpeg.h>

namespace Component {
  Video::Video(IGE::Assets::GUID _guid) : buffer{}, texture{}, videoSource{}, guid{},
    renderType{ RenderType::WORLD },
    started{ false }, paused{ false }, playOnStart{ true }
  {

  }

  Video::Video(Video const& rhs) : buffer{}, texture{}, videoSource{}, guid{ rhs.guid },
    renderType{ rhs.renderType }, started{ false }, paused{ false },
    playOnStart{ rhs.playOnStart }, audioEnabled{ rhs.audioEnabled }
  {

  }

  Video& Video::operator=(Video const& rhs) {
    renderType = rhs.renderType;
    playOnStart = rhs.playOnStart;
    guid = rhs.guid;
    audioEnabled = rhs.audioEnabled;

    return *this;
  }

  void Video::InitVideoSource(IGE::Assets::GUID _guid) {
    Release();

    std::string const& path{ 
      IGE_ASSETMGR.GetAsset<IGE::Assets::VideoAsset>(
        IGE_ASSETMGR.LoadRef<IGE::Assets::VideoAsset>(_guid)
      )->mVideoPath
    };

    videoSource = plm_create_with_filename(path.c_str());
    if (!videoSource) {
      throw Debug::Exception<Video>(Debug::LVL_ERROR, 
        Msg("Unable to create video buffer for " + path));
    }

    plm_set_video_decode_callback(
      videoSource,
      [](plm_t* self, plm_frame_t* frame, void* user) {
        if (!frame) {
          IGE_DBGLOGGER.LogError("[Video] Unable to decode video frame");
          return;
        }

        Video* video{ reinterpret_cast<Video*>(user) };

        //// if the video didn't advance, simply render the same frame
        //if (video->GetVideoTimestamp() == video->prevTimestamp) {
        //  return;
        //}

        //IGE_DBGLOGGER.LogInfo("Frame decoded! Timestamp: " + std::to_string(video->GetVideoTimestamp()));

        plm_frame_to_rgb(frame, video->buffer.data(), frame->width * 3);

        GLCALL(glTextureSubImage2D(
          video->texture->GetTexHdl(),
          0, 0, 0,
          frame->width, frame->height,
          GL_RGB, GL_UNSIGNED_BYTE, video->buffer.data())
        );
      },
      this
    );

    texture = std::make_unique<Graphics::Texture>(
      static_cast<uint32_t>(plm_get_width(videoSource)),
      static_cast<uint32_t>(plm_get_height(videoSource)),
      GL_RGB8 // not sure if we need alpha channel
    );
    buffer.resize(texture->GetWidth() * texture->GetHeight() * 3);
    
    // set the image to the first frame as a preview
    bool const originalAudioEnabled{ audioEnabled };
    if (originalAudioEnabled) {
      EnableAudio(false);
    }

    plm_frame_t* frame{ plm_decode_video(videoSource) };
    if (!frame) {
      throw Debug::Exception<Video>(Debug::LVL_ERROR,
        Msg("Unable to decode video frame from " + path));
    }
    plm_frame_to_rgb(frame, buffer.data(), frame->width * 3);
    GLCALL(glTextureSubImage2D(
      texture->GetTexHdl(),
      0, 0, 0,
      frame->width, frame->height,
      GL_RGB, GL_UNSIGNED_BYTE, buffer.data())
    );
    plm_rewind(videoSource); // reset it back to default

    EnableAudio(originalAudioEnabled);
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
  }

  void Video::EnableAudio(bool enabled) {
    plm_set_audio_enabled(videoSource, enabled);
    audioEnabled = enabled;
  }

  bool Video::IsLoopEnabled() const {
    return static_cast<bool>(plm_get_loop(videoSource));
  }

  void Video::SetLoop(bool enabled) {
    plm_set_loop(videoSource, enabled);
  }

  void Video::Release() {
    buffer.clear();
    texture.reset();
    if (videoSource) {
      plm_destroy(videoSource);
      videoSource = nullptr;
      IGE_DBGLOGGER.LogInfo("Destroyedddd");
    }
  }

  void Video::Clear() noexcept {
    Release();

    renderType = RenderType::WORLD;
    guid = {};
    playOnStart = true;
    started = paused = false;
  }

  Video::~Video() {
    Release();
  }
}
