#pragma once
#include <memory>
#include "Pipeline.h"
#include "Texture.h"
#include "Core/Entity.h"

namespace Graphics {
    class EditorCamera;

    struct RenderPassSpec
    {
        std::shared_ptr<Pipeline> pipeline;
        std::string debugName;
        glm::vec4 markerColor;
    };

    class RenderPass : public std::enable_shared_from_this<RenderPass>{
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
        void Render(EditorCamera const& camera, std::vector<ECS::Entity> const& entities); //Change this to take something like scene data instead of entities

        //std::shared_ptr<Texture> GetOutput(uint32_t index);

        static std::shared_ptr<RenderPass> Create(const RenderPassSpec& spec);

    private:
        RenderPassSpec mSpec{};
    };
}