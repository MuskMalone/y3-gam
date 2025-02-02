#pragma once
#include "Graphics/PostProcessing/ParticleManager.h"

namespace Component {
	struct EmitterSystem {
		std::vector<Graphics::EmitterInstance> emitters;
		void Clear() noexcept;
		void AddEmitter();
		void RemoveEmitter(uint64_t idx);
		std::vector<Graphics::EmitterInstance> const& GetEmitters();
	};
}