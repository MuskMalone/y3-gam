#pragma once
#include "RenderPass.h"

namespace Graphics {
	class PostProcessingPass : public RenderPass {
	public:
		PostProcessingPass(const RenderPassSpec& spec);
		void Render(CameraSpec const& cam, std::vector<ECS::Entity> const& entities) override;
		mutable std::shared_ptr<Graphics::Texture> mPositionGBuffer;

	private:
		std::shared_ptr<Graphics::Framebuffer> mPingPongBuffer;
	};
}