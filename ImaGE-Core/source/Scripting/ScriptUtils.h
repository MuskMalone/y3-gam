#pragma once
/*!*********************************************************************
\file   ScriptUtils.h
\author han.q\@digipen.edu
\date   5 October 2024
\brief
	This file contains the Enums and utils function required to transfer
	data between C# and C++

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#pragma once
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/object.h>
#include <memory>
#include <rttr/variant.h>
#include <vector>
#include "Core/Entity.h"

namespace Mono {

	enum ScriptFieldType
	{
		S_VOID = 0,
		UINT,
		BOOL,
		SHORT,
		INT,
		FLOAT,
		DOUBLE,
		LONG,
		USHORT,
		ULONG,
		STRING,
		VEC3,
		DVEC3,
		INT_ARR,
		FLOAT_ARR,
		DOUBLE_ARR,
		STRING_ARR,
		UINT_ARR,
		ENTITY_ARR,
		ENTITY,
		TEST,
		INSIDE,
		INSIDEB,
		PLAYERMOVE,
		DIALOGUE,
		PLAYERINTERACTION,
		INVENTORY,
		SPECIALDIALOGUE
	};


	/*!**********************************************************************
	* This struct contains the info of a C# field. Info like data type,name *
	* and pointer to it is stored in this struct. When we want to get an		*
	* an instance of it, we will use this struct to do it									  *
	************************************************************************/
	struct ScriptFieldInfo
	{
		ScriptFieldType mFieldType;
		std::string mFieldName;
		MonoClassField* mClassField; //I didn't use shared ptr for this because Mono frees this memory by itself behind the scene, using its own function (Which we do not have access to). If i were to put this in a shared ptr, it will cause an error
	};


	/*!**********************************************************************
	* This struct contains the info of a C# Class.Info like pointer to the  *
	* c# class and a map of all its field info are stored in this struct	  *																									*
	************************************************************************/
	struct ScriptClassInfo
	{
		MonoClass* mScriptClass{ nullptr };
		std::map<std::string, ScriptFieldInfo> mScriptFieldMap;
	};




	//template<std::size_t M, std::size_t N>
	//struct CSClass
	//{

	//};






	/*!*********************************************************************
	\brief
		Template along with its various specializations to get the mono
		version of an array based on the type specified
	\param MonoDomain* md
		Pointer to the MonoDomain object
	\param size_t sz
		The size of the array
	************************************************************************/
	template <typename T>
	MonoArray* GetMonoArray(std::shared_ptr<MonoDomain> md, size_t sz);

	template <>
	MonoArray* GetMonoArray<int>(std::shared_ptr<MonoDomain> md, size_t sz);

	template <>
	MonoArray* GetMonoArray<std::string>(std::shared_ptr<MonoDomain> md, size_t sz);

	template <>
	MonoArray* GetMonoArray<unsigned>(std::shared_ptr<MonoDomain> md, size_t sz);

	template <>
	MonoArray* GetMonoArray<float>(std::shared_ptr<MonoDomain> md, size_t sz);

	template <>
	MonoArray* GetMonoArray<double>(std::shared_ptr<MonoDomain> md, size_t sz);

	template <>
	MonoArray* GetMonoArray<double>(std::shared_ptr<MonoDomain> md, size_t sz);

	template <>
	MonoArray* GetMonoArray<MonoObject*>(std::shared_ptr<MonoDomain> md, size_t sz);

}
