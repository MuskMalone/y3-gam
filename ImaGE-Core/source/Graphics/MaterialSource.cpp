#include <pch.h>
#include "MaterialSource.h"

namespace Graphics {

	MaterialSource::MaterialSource(std::shared_ptr<Shader> const& shader, std::string const& name) 
		: mShader{ shader }, mName{ name }, mFlags{ static_cast<uint32_t>(MaterialFlag::DEPTH_TEST) } {
		if (!shader) {
			throw std::invalid_argument("Shader cannot be null!"); //@TODO change this to catch exception?
		}
	}

	std::shared_ptr<MaterialSource> MaterialSource::Create(std::shared_ptr<Shader>const &shader) {
		if (!shader) {
			throw std::invalid_argument("Shader cannot be null!");
		}
		return std::make_shared<MaterialSource>(shader);
	}

	bool MaterialSource::GetFlag(MaterialFlag flag) const {
		return (mFlags & static_cast<uint32_t>(flag)) != 0;
	}

	void MaterialSource::SetFlag(MaterialFlag flag, bool value) {
		if (value) {
			mFlags |= static_cast<uint32_t>(flag);
		}
		else {
			mFlags &= ~static_cast<uint32_t>(flag);
		}
	}

	// Default Texture
	std::shared_ptr<Texture> MaterialSource::GetAlbedoMap() const { return mAlbedoMap; }
	void MaterialSource::SetAlbedoMap(std::shared_ptr<Texture> texture) { mAlbedoMap = texture; }

	std::shared_ptr<Texture> MaterialSource::GetNormalMap() const { return mNormalMap; }
	void MaterialSource::SetNormalMap(std::shared_ptr<Texture> texture) { mNormalMap = texture; }

	std::shared_ptr<Texture> MaterialSource::GetMetalnessMap() const { return mMetalnessMap; }
	void MaterialSource::SetMetalnessMap(std::shared_ptr<Texture> texture) { mMetalnessMap = texture; }

	std::shared_ptr<Texture> MaterialSource::GetRoughnessMap() const { return mRoughnessMap; }
	void MaterialSource::SetRoughnessMap(std::shared_ptr<Texture> texture) { mRoughnessMap = texture; }
}