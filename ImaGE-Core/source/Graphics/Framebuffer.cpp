#include <pch.h>
#include "Framebuffer.h"
#include "Utils.h"

namespace Graphics
{

    Framebuffer::Framebuffer(FramebufferSpec const& spec) : mSpec{spec} {
        for (auto const& atSpec : mSpec.attachments.attachments) {
            if (!Utils::Framebuffer::IsDepthFormat(atSpec.textureFormat))
                mColorAttachmentSpecs.emplace_back(atSpec);
            else
                mDepthAttachmentSpec = atSpec;
        }

        Recreate();
    }
    Framebuffer::~Framebuffer() {
        glDeleteFramebuffers(1, &mFbo);
        GLCALL(glDeleteTextures(static_cast<GLsizei>(mColorAttachments.size()), mColorAttachments.data()));
        GLCALL(glDeleteTextures(1, &mDepthAttachment));
    }

    void Framebuffer::Recreate() {
        if (mFbo) {
            glDeleteFramebuffers(1, &mFbo);
            glDeleteTextures(static_cast<GLsizei>(mColorAttachments.size()), mColorAttachments.data());
            glDeleteTextures(1, &mDepthAttachment);

            mColorAttachments.clear();
            mDepthAttachment = 0;
        }

        GLCALL(glCreateFramebuffers(1, &mFbo));
        GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, mFbo));

        if (mColorAttachmentSpecs.size()) {
            mColorAttachments.resize(mColorAttachmentSpecs.size());
            Utils::Framebuffer::CreateTextures(mColorAttachments.data(), static_cast<unsigned int>(mColorAttachments.size()));

            for (int i{}; i < mColorAttachments.size(); ++i) {
                Utils::Framebuffer::BindTexture(mColorAttachments[i]);
                switch (mColorAttachmentSpecs[i].textureFormat) {
                case FramebufferTextureFormat::RGBA8:
                    Utils::Framebuffer::AttachColorTexture(mColorAttachments[i], GL_RGBA8, GL_RGBA, mSpec.width, mSpec.height, static_cast<int>(i));
                    break;
                case FramebufferTextureFormat::RED_INTEGER:
                    Utils::Framebuffer::AttachColorTexture(mColorAttachments[i], GL_R32I, GL_RED_INTEGER, mSpec.width, mSpec.height, static_cast<int>(i));
                    break;
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
            case FramebufferTextureFormat::SHADOW_MAP:
              Utils::Framebuffer::AttachShadowMapTexture(mDepthAttachment, mSpec.width, mSpec.height);
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
        GLenum status;
        if (status = glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            //log
            std::cout << "Framebuffer not complete! " << status << std::endl;
        }
//            LoggingSystem::GetInstance().Log(LogLevel::ERROR_LEVEL, "ERROR: Framebuffer is INCOMPLETE!", __FUNCTION__);
//        else LoggingSystem::GetInstance().Log(LogLevel::INFO_LEVEL, "Framebuffer is complete!", __FUNCTION__);
//#endif

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Framebuffer::Bind() {
        GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, mFbo));
        GLCALL(glViewport(0, 0, mSpec.width, mSpec.height));
    }
    void Framebuffer::Unbind() {
        GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    }

    void Framebuffer::BindDefault() {
        GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    }

    void Framebuffer::Resize(uint32_t width, uint32_t height) {
        mSpec.width = width;
        mSpec.height = height;

        Recreate();
    }

    int Framebuffer::ReadPixel(uint32_t attachIdx, int x, int y) {
        GLCALL(glReadBuffer(GL_COLOR_ATTACHMENT0 + attachIdx));
        int pixels;
        GLCALL(glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixels));
        return pixels;
    }

    void Framebuffer::ClearAttachmentInt(uint32_t attachIdx, int val) {
        GLCALL(glClearTexImage(mColorAttachments[attachIdx], 0, GL_RED_INTEGER, GL_INT, &val));
    }


    uint32_t Framebuffer::GetColorAttachmentID(uint32_t index) const {
        if (index >= static_cast<uint32_t>(mColorAttachments.size())) {
          throw Debug::Exception<Framebuffer>(Debug::LVL_ERROR, Msg("Out of bounds"));
        }
        return mColorAttachments[index];
    }

    uint32_t Framebuffer::GetDepthAttachmentID() const noexcept {
      return mDepthAttachment;
    }

    FramebufferSpec const& Framebuffer::GetFramebufferSpec() const {
        return mSpec;
    }

    std::shared_ptr<Framebuffer> Framebuffer::Create(FramebufferSpec const& spec) {
        return std::make_shared<Framebuffer>(spec);
    }
} // namespace Graphics
