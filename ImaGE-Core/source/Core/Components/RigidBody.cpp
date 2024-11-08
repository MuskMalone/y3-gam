#include <pch.h>
#include "RigidBody.h"
#include "Physics/PhysicsSystem.h"
#include "Physics/PhysicsHelpers.h"
namespace Component {
	void RigidBody::SetImpulse(glm::vec3 const& impulse)
	{
		auto& phySys{ *IGE::Physics::PhysicsSystem::GetInstance() };
		auto pxrb{ phySys.mRigidBodyIDs.at(bodyID) };
		pxrb->addForce(IGE::Physics::ToPxVec3(impulse), physx::PxForceMode::eIMPULSE);
	}
	void RigidBody::SetForce(glm::vec3 const& force)
	{
		auto& phySys{ *IGE::Physics::PhysicsSystem::GetInstance() };
		auto pxrb{ phySys.mRigidBodyIDs.at(bodyID) };
		pxrb->addForce(IGE::Physics::ToPxVec3(force));
	}
	void RigidBody::SetPosition(glm::vec3 const& pos)
	{
		auto& phySys{ *IGE::Physics::PhysicsSystem::GetInstance() };
		auto pxrb{ phySys.mRigidBodyIDs.at(bodyID) };
		auto pose{ pxrb->getGlobalPose() };
		pxrb->setGlobalPose({ IGE::Physics::ToPxVec3(pos), pose.q });
	}
	void RigidBody::SetAxisLock(int lock)
	{
		axisLock |= lock;
	}
	void RigidBody::RemoveAxisLock(int lock)
	{
		int removeMask = -1 ^ lock;
		axisLock &= removeMask;
	}
	bool RigidBody::IsAxisLocked(int axis)
	{
		return static_cast<bool>(axisLock & axis);
	}
	void RigidBody::SetAngleAxisLock(int lock)
	{
		angularAxisLock |= lock;
	}
	void RigidBody::RemoveAngleAxisLock(int lock)
	{
		int removeMask = -1 ^ lock;
		angularAxisLock &= removeMask;
	}
	bool RigidBody::IsAngleAxisLocked(int axis)
	{
		return static_cast<bool>(angularAxisLock & axis);
	}
}