/*!*********************************************************************
\file   RenderPass.h
\author k.choa\@digipen.edu
\date   22/09/2024
\brief  The RenderPass class handles the configuration and execution of a single rendering pass, 
        which includes binding the pipeline, setting up camera and shader uniforms, and submitting 
        entities to the renderer for rendering. It supports instanced rendering and material application for entities with materials.

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#pragma once
#include <memory>
#include <vector>
#include <Graphics/Pipeline.h>
#include <Graphics/Camera/CameraSpec.h>

namespace ECS { class Entity; }

namespace Graphics {
    struct EditorCamera;

    struct RenderPassSpec {
        std::shared_ptr<Pipeline> pipeline;
        std::string debugName;
        glm::vec4 markerColor;
    };

    class RenderPass : public std::enable_shared_from_this<RenderPass> {
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
  
        virtual void Render(CameraSpec const& cam, std::vector<ECS::Entity> const& entities) = 0;

        inline void SetInputTexture(std::shared_ptr<Texture> const& tex) { mInputTexture = tex; };
        inline std::shared_ptr<Texture> const& GetOutputTexture() { return mOutputTexture; };

        //std::shared_ptr<Texture> GetOutput(uint32_t index);

        template <typename T>
        static std::shared_ptr<T> Create(const RenderPassSpec& spec) { return std::make_shared<T>(spec); }

    protected:
        RenderPassSpec mSpec{};
        std::shared_ptr<Texture> mInputTexture; 
        std::shared_ptr<Texture> mOutputTexture;
    };
} // namespace Graphics
