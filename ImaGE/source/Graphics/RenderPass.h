#pragma once
#include "Pipeline.h"
#include "Texture.h"

namespace Graphics {
    struct RenderPassSpec
    {
        std::shared_ptr<Pipeline> pipeline;
        std::string debugName;
        glm::vec4 markerColor;
    };

    class RenderPass {
    public:
        RenderPass(const RenderPassSpec& spec);
        RenderPassSpec& GetSpecification();
        RenderPassSpec const& GetSpecification() const;

        std::shared_ptr<Pipeline> GetPipeline() const;
        std::shared_ptr<Framebuffer> GetTargetFramebuffer() const;

        //bool Validate();
        //void Bake();
        //bool Baked() const;
        //void Prepare();
        void Begin();
        void End();

        //std::shared_ptr<Texture> GetOutput(uint32_t index);

        static std::shared_ptr<RenderPass> Create(const RenderPassSpec& spec);

    private:
        RenderPassSpec mSpec{};
    };
}