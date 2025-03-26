#include <pch.h>
#include "Video.h"
#include <Asset/AssetManager.h>
#include <Asset/Assetables/Video/VideoAsset.h>

#define PL_MPEG_IMPLEMENTATION
#include <pl_mpeg.h>

namespace Component {
  Video::Video(IGE::Assets::GUID _guid) : buffer{}, texture{}, videoSource{}, guid{},
    renderType{ RenderType::WORLD },
    timeElapsed{}, started{ false }, paused{ false }, playOnStart{ true }
  {
    if (guid) {
      InitVideoSource(guid);
    }
  }

  Video::Video(Video const& rhs) : buffer{}, texture{}, videoSource{}, guid{ rhs.guid },
    renderType{ rhs.renderType },
    timeElapsed{}, started{ false }, paused{ false }, playOnStart{ rhs.playOnStart }
  {
    if (guid) {
      InitVideoSource(guid);
    }
  }

  Video& Video::operator=(Video const& rhs) {
    Clear();
    renderType = rhs.renderType;
    playOnStart = rhs.playOnStart;
    if (guid) {
      InitVideoSource(guid);
    }

    return *this;
  }

  void Video::InitVideoSource(IGE::Assets::GUID _guid) {
    Clear();

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
        Video* video{ reinterpret_cast<Video*>(user) };

        // if the video didn't advance, simply render the same frame
        if (video->GetVideoTimestamp() == video->prevTimestamp) {
          return;
        }

        plm_frame_to_rgb(frame, video->buffer.data(), frame->width * 3);

        glBindTexture(GL_TEXTURE_2D, video->texture->GetTexHdl());
        glTexImage2D(
          GL_TEXTURE_2D, 0, GL_RGB, frame->width, frame->height, 0,
          GL_RGB, GL_UNSIGNED_BYTE, video->buffer.data()
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
    plm_frame_t* frame{ plm_decode_video(videoSource) };
    if (!frame) {
      throw Debug::Exception<Video>(Debug::LVL_ERROR,
        Msg("Unable to decode video frame from " + path));
    }
    plm_frame_to_rgb(frame, buffer.data(), frame->width * 3);
    glBindTexture(GL_TEXTURE_2D, texture->GetTexHdl());
    glTexImage2D(
      GL_TEXTURE_2D, 0, GL_RGB, frame->width, frame->height, 0,
      GL_RGB, GL_UNSIGNED_BYTE, buffer.data()
    );
    plm_rewind(videoSource); // reset it back to default

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
    prevTimestamp = GetVideoTimestamp();
    timeElapsed += seconds;
    plm_decode(videoSource, timeElapsed);
  }

  bool Video::IsAudioEnabled() const { 
    return static_cast<bool>(plm_get_audio_enabled(videoSource));
  }

  void Video::EnableAudio(bool enabled) {
    plm_set_audio_enabled(videoSource, enabled);
  }

  bool Video::IsLoopEnabled() const {
    return static_cast<bool>(plm_get_loop(videoSource));
  }

  void Video::SetLoop(bool enabled) {
    plm_set_loop(videoSource, enabled);
  }

  void Video::Clear() noexcept {
    buffer.clear();
    texture.reset();
    if (videoSource) {
      plm_destroy(videoSource);
      videoSource = nullptr;
    }

    renderType = RenderType::WORLD;
    guid = {};
    prevTimestamp = timeElapsed = 0.f;
    playOnStart = true;
    started = paused = false;
  }

  Video::~Video() {
    if (videoSource) {
      plm_destroy(videoSource);
    }
  }
}
