#pragma once
#include "RenderPass.h"

namespace Graphics {
	class ParticlePass : public RenderPass {
	public:
		ParticlePass(const RenderPassSpec& spec);
		void Render(CameraSpec const& cam, std::vector<ECS::Entity> const& entities) override;
	private:
	};
}