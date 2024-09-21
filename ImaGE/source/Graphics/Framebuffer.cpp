#include <pch.h>
#include "Framebuffer.h"
#include "Utils.h"

namespace Graphics
{
    namespace Temp {
        Framebuffer::Framebuffer(unsigned width, unsigned height) : mWidth{ width }, mHeight{ height }
        {
            // create and bind
            glGenFramebuffers(1, &mFBO);
            glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
            glGenTextures(1, &mTexture);
            glBindTexture(GL_TEXTURE_2D, mTexture);

            // generate texture
            glGenTextures(1, &mTexture);
            glBindTexture(GL_TEXTURE_2D, mTexture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            // attach it to currently bound framebuffer object
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mTexture, 0);

            glGenRenderbuffers(1, &mRBO);
            glBindRenderbuffer(GL_RENDERBUFFER, mRBO);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mRBO);

            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
                throw std::runtime_error("Framebuffer is not complete!");
            }

            // unbind buffers
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glBindTexture(GL_TEXTURE_2D, 0);
            glBindRenderbuffer(GL_RENDERBUFFER, 0);
        }

        void Framebuffer::Resize(unsigned width, unsigned height)
        {
            mWidth = width;
            mHeight = height;

            glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
            glBindTexture(GL_TEXTURE_2D, mTexture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mTexture, 0);

            glBindRenderbuffer(GL_RENDERBUFFER, mRBO);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mRBO);

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        void Framebuffer::Release()
        {
            glDeleteFramebuffers(1, &mFBO);
            glDeleteTextures(1, &mTexture);
            glDeleteRenderbuffers(1, &mRBO);
        }
    }

    Framebuffer::Framebuffer(FramebufferSpec const&) {
        for (auto const& spec : mSpec.attachments.attachments) {
            if (!Utils::Framebuffer::IsDepthFormat(spec.textureFormat))
                mColorAttachmentSpecs.emplace_back(spec);
            else
                mDepthAttachmentSpec = spec;
        }

        Recreate();
    }
    Framebuffer::~Framebuffer() {
        glDeleteFramebuffers(1, &mFbo);
        glDeleteTextures(static_cast<GLsizei>(mColorAttachments.size()), mColorAttachments.data());
        glDeleteTextures(1, &mDepthAttachment);
    }

    void Framebuffer::Recreate() {
        if (mFbo) {
            glDeleteFramebuffers(1, &mFbo);
            glDeleteTextures(static_cast<GLsizei>(mColorAttachments.size()), mColorAttachments.data());
            glDeleteTextures(1, &mDepthAttachment);

            mColorAttachments.clear();
            mDepthAttachment = 0;
        }

        glCreateFramebuffers(1, &mFbo);
        glBindFramebuffer(GL_FRAMEBUFFER, mFbo);

        if (mColorAttachmentSpecs.size()) {
            mColorAttachments.resize(mColorAttachmentSpecs.size());
            Utils::Framebuffer::CreateTextures(mColorAttachments.data(), static_cast<unsigned int>(mColorAttachments.size()));

            for (int i{}; i < mColorAttachments.size(); ++i) {
                Utils::Framebuffer::BindTexture(mColorAttachments[i]);
                switch (mColorAttachmentSpecs[i].textureFormat) {
                case FramebufferTextureFormat::RGBA8:
                    Utils::Framebuffer::AttachColorTexture(mColorAttachments[i], GL_RGBA8, GL_RGBA, mSpec.width, mSpec.height, static_cast<int>(i));
                    break;
                //case FramebufferTextureFormat::RED_INTEGER:
                //    Utils::Framebuffer::AttachColorTexture(mColorAttachments[i], GL_R32I, GL_RED_INTEGER, mSpec.width, mSpec.height, static_cast<int>(i));
                //    break;
                }
            }
        }

        if (mDepthAttachmentSpec.textureFormat != FramebufferTextureFormat::NONE) {
            Utils::Framebuffer::CreateTextures(&mDepthAttachment, 1);
            Utils::Framebuffer::BindTexture(mDepthAttachment);
            switch (mDepthAttachmentSpec.textureFormat) {
            case FramebufferTextureFormat::DEPTH24STENCIL8:
                Utils::Framebuffer::AttachDepthTexture(mDepthAttachment, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT, mSpec.width, mSpec.height);
                break;
            }
        }

        if (mColorAttachments.size() > 1) {
            if (mColorAttachments.size() > 4) {
//#ifndef _INSTALLER
//                LoggingSystem::GetInstance().Log(LogLevel::ERROR_LEVEL, "TOO MANY ATTACHMENTS!", __FUNCTION__);
//#endif
                throw std::runtime_error("TOO MANY ATTACHMENTS");
            }

            GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
            glDrawBuffers(static_cast<GLsizei>(mColorAttachments.size()), buffers);
        }
        else if (mColorAttachments.empty()) {
            glDrawBuffer(GL_NONE); // only depth
        }
//#ifndef _INSTALLER
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            //log
        }
//            LoggingSystem::GetInstance().Log(LogLevel::ERROR_LEVEL, "ERROR: Framebuffer is INCOMPLETE!", __FUNCTION__);
//        else LoggingSystem::GetInstance().Log(LogLevel::INFO_LEVEL, "Framebuffer is complete!", __FUNCTION__);
//#endif

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Framebuffer::Bind() {
        glBindFramebuffer(GL_FRAMEBUFFER, mFbo);
        glViewport(0, 0, mSpec.width, mSpec.height);
    }
    void Framebuffer::Unbind() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Framebuffer::Resize(uint32_t width, uint32_t height) {
        mSpec.width = width;
        mSpec.height = height;

        Recreate();
    }
    uint32_t Framebuffer::GetColorAttachmentID(uint32_t index) const {
        if (index >= mColorAttachments.size()) { throw std::runtime_error("out of bounds"); } return mColorAttachments[index]; // TODO CHANGE LOG
    }
    FramebufferSpec const& Framebuffer::GetFramebufferSpec() const {
        return mSpec;
    }

    std::shared_ptr<Framebuffer> Framebuffer::Create(FramebufferSpec const& spec) {
        return std::make_shared<Framebuffer>(spec);
    }
} // namespace Graphics
