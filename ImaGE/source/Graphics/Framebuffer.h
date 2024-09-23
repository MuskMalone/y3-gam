#pragma once
#include <glad/glad.h>
#include <string>

namespace Graphics
{

    enum class FramebufferTextureFormat {
        NONE = 0,
        RGBA8,
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
        uint32_t GetColorAttachmentID(uint32_t index = 0) const;
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

    namespace Temp {
        class Framebuffer
        {
        public:
            Framebuffer(unsigned width, unsigned height);

            inline GLuint GetTextureID() const noexcept { return mTexture; }
            inline void Bind() const { glBindFramebuffer(GL_FRAMEBUFFER, mFBO); }
            inline void Unbind() const { glBindFramebuffer(GL_FRAMEBUFFER, 0); }
            void Resize(unsigned width, unsigned height);
            void Release();

        private:
            GLuint mFBO, mRBO;
            GLuint mTexture;
            unsigned mWidth, mHeight;
        };
    }

} // namespace Graphics
