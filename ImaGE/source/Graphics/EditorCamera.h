#pragma once
#include "Utils.h"

namespace Graphics {
	class EditorCamera {
	public:
		glm::mat4 GetViewMatrix() const;
		glm::mat4 GetProjMatrix() const;
		glm::mat4 GetViewProjMatrix() const;
	private:
		glm::vec3 mPosition;
		float mYaw;
		float mPitch;
		float mFov;
		float mAspectRatio;
		float mNearClip;
		float mFarClip;

		float mMoveSpeed{2.5f};
		float mMouseSense{ 0.1f };
	};
}