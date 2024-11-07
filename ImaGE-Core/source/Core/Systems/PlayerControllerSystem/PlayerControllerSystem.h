#pragma once
#include <Core/Systems/System.h>
#include <Core/Systems/SystemManager/SystemManager.h>


namespace Systems {

	

	class PlayerControllerSystem : public System {
	public:
		PlayerControllerSystem(const char* name);

		void Start() override;
		void Update() override;
		void Destroy() override;


	private:


	private:


		//EVENT_CALLBACK_DECL(OnSceneLoad);

	};

}
