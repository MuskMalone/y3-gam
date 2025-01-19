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
//#include <Core/Coordinator.hpp>
//#include <Core/Serialization/Serializer.hpp>
#include <any>
#include "Core/Components/Transform.h"
#include "Core/EntityManager.h"
#include "Core/Entity.h"


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
	public:
		CommandManager();

		template <typename... _args>
		void AddCommand(std::string const& cmd, _args... args) {
			mCommandStack.push_back(CommandBlock{ cmd, args... });
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
		std::deque<CommandBlock> mCommandStack{};
		std::map<std::string, std::function<void(ParamPack const&)>> mCommandLookup{ {"Transform",TransformCmd}};
	};
}