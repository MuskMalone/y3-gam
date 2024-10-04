#include <pch.h>
#include "MaterialSource.h"

namespace Graphics {

	MaterialSource::MaterialSource(std::shared_ptr<Shader> const& shader, std::string const& name) 
		: mShader{ shader }, mName{ name }, mFlags{ static_cast<uint32_t>(MaterialFlag::DEPTH_TEST) } {
		if (!shader) {
			throw std::invalid_argument("Shader cannot be null!"); //@TODO change this to catch exception? or change to error log?
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
	//std::shared_ptr<Texture> 
	IGE::Assets::GUID const& MaterialSource::GetAlbedoMap() const { return mAlbedoMap; }
	IGE::Assets::GUID const& MaterialSource::SetAlbedoMap(IGE::Assets::GUID const& texture) { return mAlbedoMap = texture; }

	IGE::Assets::GUID MaterialSource::GetNormalMap() const { return mNormalMap; }
	void MaterialSource::SetNormalMap(IGE::Assets::GUID const& texture) { mNormalMap = texture; }

	IGE::Assets::GUID MaterialSource::GetMetalnessMap() const { return mMetalnessMap; }
	void MaterialSource::SetMetalnessMap(IGE::Assets::GUID const& texture) { mMetalnessMap = texture; }

	IGE::Assets::GUID MaterialSource::GetRoughnessMap() const { return mRoughnessMap; }
	void MaterialSource::SetRoughnessMap(IGE::Assets::GUID const& texture) { mRoughnessMap = texture; }
}