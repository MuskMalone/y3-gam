#pragma once
#include <Core/Systems/System.h>

namespace Mono
{
	class ScriptingSystem : public Systems::System{
		
	public:

		ScriptingSystem(const char* name) : System(name)  {}
		void Update();

	};
}

