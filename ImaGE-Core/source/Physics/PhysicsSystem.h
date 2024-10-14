#pragma once
#include <Core/Components/Components.h>
#include <Core/Entity.h>
#include <Core/Systems/SystemManager/SystemManager.h>

namespace IGE {
	namespace Physics {
		const float gDeltaTime = 1.f / 60.f;

		class PhysicsSystem : public Systems::System {
		public:
			static std::shared_ptr<IGE::Physics::PhysicsSystem> GetInstance();
			PhysicsSystem();
			~PhysicsSystem();
			void Update() override;

			// temporarily using this to deserialize a copy of the rb from file
			Component::RigidBody& AddRigidBody(ECS::Entity entity, Component::RigidBody rb = {}); // should const ref this when the functions r consted
			void ChangeRigidBodyVar(ECS::Entity entity, Component::RigidBodyVars var);

			// temporarily using this to deserialize a copy of the collider from file
			Component::Collider& AddCollider(ECS::Entity entity, Component::Collider collider = {});
			void ChangeColliderShape(ECS::Entity entity);
			void ChangeColliderVar(ECS::Entity entity, Component::ColliderVars var);

			void Debug(float dt); // to be called within rendersystems geom pass
		//private:
		//	const uint32_t cMaxBodies = 65536;
		//	const uint32_t cNumBodyMutexes = 0;
		//	const uint32_t cMaxBodyPairs = 65536;
		//	const uint32_t cMaxContactConstraints = 10240;

		private:
			//JPH::TempAllocatorImpl mTempAllocator;
			//JPH::JobSystemThreadPool mJobSystem;
			//BPLayerInterfaceImpl mBroadPhaseLayerInterface;
			//ObjectVsBroadPhaseLayerFilterImpl mObjectVsBroadphaseLayerFilter;
			//ObjectLayerPairFilterImpl mObjectVsObjectLayerFilter;

			//JPH::PhysicsSystem mPhysicsSystem;
			//MyBodyActivationListener mBodyActivationListener;
			//MyContactListener mContactListener;
			physx::PxDefaultAllocator      mAllocator;
			physx::PxDefaultErrorCallback  mErrorCallback;
			physx::PxFoundation* mFoundation = nullptr;
			physx::PxPvd* mPvd = nullptr;
			physx::PxPhysics* mPhysics = nullptr;
			physx::PxMaterial* mMaterial = nullptr;
			physx::PxScene* mScene = nullptr;
			// just the pointers returned from createdynamic but in void* form
			// map is a way to get around the pesky casting
			std::unordered_map<void*, physx::PxRigidDynamic*> mRigidBodyIDs; 
			static std::shared_ptr<IGE::Physics::PhysicsSystem> _mSelf;
			static std::mutex _mMutex;
			PhysicsSystem(PhysicsSystem& other) = delete;
			void operator=(const PhysicsSystem&) = delete;
		};
	}
}
template <>
inline void Systems::SystemManager::RegisterSystem<IGE::Physics::PhysicsSystem>(const char* name) {
	SystemPtr sys{ IGE::Physics::PhysicsSystem::GetInstance() };
	mNameToSystem.emplace(typeid(IGE::Physics::PhysicsSystem).name(), sys);
	mSystems.emplace_back(std::move(sys));
}

//
////for testing tch
//namespace GUI {
//	class Inspector;
//} //forward decl
//template <>
//inline void GUI::Inspector::DrawAddComponentButton<Component::RigidBody>(std::string const& name, std::string const& icon) {
//	if (GUIManager::GetSelectedEntity().HasComponent<Component::RigidBody>()) {
//		return;
//	}
//
//	auto fillRowWithColour = [](const ImColor& colour) {
//		for (int column = 0; column < ImGui::TableGetColumnCount(); column++) {
//			ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, colour, column);
//		}
//	};
//
//	const float rowHeight = 25.0f;
//	auto* window = ImGui::GetCurrentWindow();
//	window->DC.CurrLineSize.y = rowHeight;
//	ImGui::TableNextRow(0, rowHeight);
//	ImGui::TableSetColumnIndex(0);
//
//	window->DC.CurrLineTextBaseOffset = 3.0f;
//
//	const ImVec2 rowAreaMin = ImGui::TableGetCellBgRect(ImGui::GetCurrentTable(), 0).Min;
//	const ImVec2 rowAreaMax = { ImGui::TableGetCellBgRect(ImGui::GetCurrentTable(),
//	  ImGui::TableGetColumnCount() - 1).Max.x, rowAreaMin.y + rowHeight };
//
//	//ImGui::GetWindowDrawList()->AddRect(rowAreaMin, rowAreaMax, Color::IMGUI_COLOR_RED); // Debug
//
//	ImGui::PushClipRect(rowAreaMin, rowAreaMax, false);
//	bool isRowHovered, isRowClicked;
//	ImGui::ButtonBehavior(ImRect(rowAreaMin, rowAreaMax), ImGui::GetID(name.c_str()),
//		&isRowHovered, &isRowClicked, ImGuiButtonFlags_MouseButtonLeft);
//	ImGui::SetItemAllowOverlap();
//	ImGui::PopClipRect();
//
//	std::string display{ icon + "   " + name};
//
//	ImGui::PushFont(mStyler.GetCustomFont(GUI::MONTSERRAT_SEMIBOLD));
//	ImGui::TextUnformatted(display.c_str());
//	ImGui::PopFont();
//
//	if (isRowHovered)
//		fillRowWithColour(Color::IMGUI_COLOR_ORANGE);
//
//	if (isRowClicked) {
//		ECS::Entity ent{ GUIManager::GetSelectedEntity().GetRawEnttEntityID() };
//		IGE::Physics::PhysicsSystem::GetInstance()->AddRigidBody(ent);
//		SetIsComponentEdited(true);
//		ImGui::CloseCurrentPopup();
//	}
//}
//
////for testing tch
//template <>
//inline void GUI::Inspector::DrawAddComponentButton<Component::Collider>(std::string const& name, std::string const& icon) {
//	if (GUIManager::GetSelectedEntity().HasComponent<Component::Collider>()) {
//		return;
//	}
//
//	auto fillRowWithColour = [](const ImColor& colour) {
//		for (int column = 0; column < ImGui::TableGetColumnCount(); column++) {
//			ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, colour, column);
//		}
//	};
//
//	const float rowHeight = 25.0f;
//	auto* window = ImGui::GetCurrentWindow();
//	window->DC.CurrLineSize.y = rowHeight;
//	ImGui::TableNextRow(0, rowHeight);
//	ImGui::TableSetColumnIndex(0);
//
//	window->DC.CurrLineTextBaseOffset = 3.0f;
//
//	const ImVec2 rowAreaMin = ImGui::TableGetCellBgRect(ImGui::GetCurrentTable(), 0).Min;
//	const ImVec2 rowAreaMax = { ImGui::TableGetCellBgRect(ImGui::GetCurrentTable(),
//	  ImGui::TableGetColumnCount() - 1).Max.x, rowAreaMin.y + rowHeight };
//
//	//ImGui::GetWindowDrawList()->AddRect(rowAreaMin, rowAreaMax, Color::IMGUI_COLOR_RED); // Debug
//
//	ImGui::PushClipRect(rowAreaMin, rowAreaMax, false);
//	bool isRowHovered, isRowClicked;
//	ImGui::ButtonBehavior(ImRect(rowAreaMin, rowAreaMax), ImGui::GetID(name.c_str()),
//		&isRowHovered, &isRowClicked, ImGuiButtonFlags_MouseButtonLeft);
//	ImGui::SetItemAllowOverlap();
//	ImGui::PopClipRect();
//
//	std::string display{ icon + "   " + name};
//
//	ImGui::PushFont(mStyler.GetCustomFont(GUI::MONTSERRAT_SEMIBOLD));
//	ImGui::TextUnformatted(display.c_str());
//	ImGui::PopFont();
//
//	if (isRowHovered)
//		fillRowWithColour(Color::IMGUI_COLOR_ORANGE);
//
//	if (isRowClicked) {
//		ECS::Entity ent{ GUIManager::GetSelectedEntity().GetRawEnttEntityID() };
//		IGE::Physics::PhysicsSystem::GetInstance()->AddCollider(ent);
//		SetIsComponentEdited(true);
//		ImGui::CloseCurrentPopup();
//	}
//}
