#pragma once
#include <utility>
#include <Core/Systems/System.h>
#include <Core/Entity.h>
#include <Core/Systems/SystemManager/SystemManager.h>

namespace Systems {

	struct Font {
		Font() = delete;
		Font(std::string const& fp);
		~Font();

		std::string mFilePath;
	};

	class TextSystem : public System {
	public:
		TextSystem(const char* name) : System(name) {}

		void Start() override;
		void Update() override;

	private:

	};

}
