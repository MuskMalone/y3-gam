/*!*********************************************************************
\file   Script.h
\author han.q\@digipen.edu
\date   5 October 2024
\brief
	This file contains the declaration of Script Component struct. This component
	has a vector that stores all the c# script attached to the entity

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#pragma once
#include <string>
#include <algorithm>
#include <Scripting/ScriptManager.h>
#include <Scripting/ScriptInstance.h>

namespace Component {

    struct Script {

      Script() = default;

			Script(std::vector<std::string> const& scriptNames)
			{
				/*for (const std::string& s : scriptNames)
				{
					std::vector<Mono::ScriptInstance>::iterator it = std::find_if(mScriptList.begin(), mScriptList.end(), [s](const Mono::ScriptInstance& script) { return script.mScriptName == s; });
					if (it == mScriptList mScriptList.end())
					{
						try
						{
							ECS::Entity garbageID{ GE::ECS::INVALID_ID };
							m_scriptList.emplace_back(s, garbageID);
						}
						catch (GE::Debug::IExceptionBase& e)
						{
							e.LogSource();
							throw GE::Debug::Exception<ScriptManager>(GE::Debug::LEVEL_ERROR, "Failed to Instantiate the class " + s, ERRLG_FUNC, ERRLG_LINE);
						}
					}

				}*/
			}

			void UpdateAllScripts()
			{
				double dt = Performance::FrameRateController::GetInstance().GetDeltaTime();
				for (Mono::ScriptInstance& cs : mScriptList)
				{
					cs.InvokeOnUpdate(dt);
				}
			}


      inline void Clear() noexcept { }
      std::vector<Mono::ScriptInstance> mScriptList;

    };
  
} // namespace Component