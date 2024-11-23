/*!*********************************************************************
\file   FrameBuffer.h
\author k.choa\@digipen.edu
\date   22/09/2024
\brief  The Framebuffer class handles the creation, binding, and management of framebuffer objects.
        It supports multiple color attachments and a depth attachment for rendering operations.
        The class also provides functionality for resizing the framebuffer, reading pixel data,
        and clearing attachment values.

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#pragma once
#include <glad/glad.h>
#include <string>

namespace Graphics
{

    enum class FramebufferTextureFormat {
        NONE = 0,
        RGB32F,
        RGBA8,
        RED_INTEGER,
        SHADOW_MAP,
        DEPTH24STENCIL8,

        DEPTH = DEPTH24STENCIL8 //default
    };

    struct FramebufferTextureSpec {
        FramebufferTextureSpec() = default;
        FramebufferTextureSpec(FramebufferTextureFormat fmt) : textureFormat{ fmt } {}

        FramebufferTextureFormat textureFormat = FramebufferTextureFormat::NONE;
    };

    struct FramebufferAttachmentSpec {
        FramebufferAttachmentSpec() = default;
        FramebufferAttachmentSpec(std::initializer_list<FramebufferTextureSpec> ats) : attachments{ ats } {}

        std::vector<FramebufferTextureSpec> attachments;
    };

    struct FramebufferSpec {
        uint32_t width{}, height{};
        uint32_t samples { 1 };
        FramebufferAttachmentSpec attachments;
    };

    class Framebuffer {
    public:
        Framebuffer(FramebufferSpec const&);
        ~Framebuffer();

        void Recreate();

        void Bind();
        void Unbind();

        static void BindDefault();

        void Resize(uint32_t width, uint32_t height);

        int ReadPixel(uint32_t attachIdx, int x, int y);
        void ClearAttachmentInt(uint32_t attachIdx, int val);

        uint32_t GetColorAttachmentID(uint32_t index = 0) const;
        uint32_t GetDepthAttachmentID() const noexcept;
        FramebufferSpec const& GetFramebufferSpec() const;

        static std::shared_ptr<Framebuffer> Create(FramebufferSpec const& spec);
    private:
        uint32_t mFbo{};
        FramebufferSpec mSpec;

        std::vector<FramebufferTextureSpec> mColorAttachmentSpecs;
        FramebufferTextureSpec mDepthAttachmentSpec = FramebufferTextureFormat::NONE;

        //IDs
        std::vector<uint32_t> mColorAttachments;
        uint32_t mDepthAttachment{};
    };

} // namespace Graphics
