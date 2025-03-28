#pragma once
/******************************************************************************/
/*!
\par        Image Engine
\file       CommandManager.hpp

\author     tan cheng hian (t.chenghian)
\date       Nov 10, 2023

\brief      CommandManager

\copyright  Copyright (C) 2023 DigiPen Institute of Technology. Reproduction
			or disclosure of this file or its contents without the prior
			written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/
#include <any>
#include "Core/Components/Transform.h"
#include "Core/EntityManager.h"
#include "Core/Entity.h"
#include <Events/EventCallback.h>

#define IGE_CMDMGR CMD::CommandManager::GetInstance()

using ParamPack = std::vector<std::any>;

namespace CMD
{
	void TransformCmd(ParamPack const&);

	class CommandManager : public ThreadSafeSingleton<CommandManager>
	{
	private:
	
		struct CommandBlock {
			std::string command;
			ParamPack params;

			//im just dicking around lol
			template <typename... _args>
			CommandBlock(std::string const& cmd, _args&&... args) : command{ cmd } {
				// Unpack the parameter pack and emplace each argument into the vector
				(params.push_back(std::any(std::forward<_args>(args))), ...);
			}
		};

		inline static constexpr unsigned sMaxCommands = 100u;

	public:
		CommandManager();

		template <typename... _args>
		void AddCommand(std::string const& cmd, _args&&... args) {
			if (mCommandStack.size() >= sMaxCommands) {
				mCommandStack.pop_front();
				IGE_DBGLOGGER.LogInfo("Popped front");
			}
			mCommandStack.emplace_back(cmd, std::forward<_args>(args)...);
		}
		inline void UndoCommand() {
			if (mCommandStack.empty()) return;
			(mCommandLookup.at(mCommandStack.back().command))(mCommandStack.back().params);
			mCommandStack.pop_back();
		}
		inline void RegisterCommand(std::string name, std::function<void(ParamPack const&)> func) {
			mCommandLookup[name] = func;
		}

	private:
		EVENT_CALLBACK_DECL(OnSceneChange);

		std::deque<CommandBlock> mCommandStack{};
		std::map<std::string, std::function<void(ParamPack const&)>> mCommandLookup{ {"Transform", TransformCmd}};
	};
}