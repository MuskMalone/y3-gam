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
#include "Core/Components/Camera.h"

namespace ECS { class Entity; }

namespace Graphics {
    class EditorCamera;

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
  
        virtual void Render(const Component::Camera& camera, std::vector<ECS::Entity> const& entities) = 0;
        virtual void Render(EditorCamera const& camera, std::vector<ECS::Entity> const& entities) = 0; //Change this to take something like scene data instead of entities

        //std::shared_ptr<Texture> GetOutput(uint32_t index);

        template <typename T>
        static std::shared_ptr<T> Create(const RenderPassSpec& spec) { return std::make_shared<T>(spec); }

    protected:
        RenderPassSpec mSpec{};
    };
} // namespace Graphics
