#pragma once
/*!*********************************************************************
\file   ScriptUtils.h
\author han.q\@digipen.edu
\date   26 November 2023
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

namespace Mono {

	enum ScriptFieldType
	{
		S_VOID = 0,
		BOOL,
		CHAR,
		SHORT,
		INT,
		FLOAT,
		DOUBLE,
		LONG,
		USHORT,
		UINT,
		ULONG,
		STRING,
		VEC2,
		VEC3,
		DVEC2,
		DVEC3,
		INT_ARR,
		FLOAT_ARR,
		DOUBLE_ARR,
		STRING_ARR,
		UINT_ARR
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
		std::shared_ptr<MonoClassField> mClassField;
	};


	/*!**********************************************************************
	* This struct contains the info of a C# Class.Info like pointer to the  *
	* c# class and a map of all its field info are stored in this struct	  *																									*
	************************************************************************/
	struct ScriptClassInfo
	{
		std::shared_ptr<MonoClass> mScriptClass{ nullptr };
		std::map<std::string, ScriptFieldInfo> mScriptFieldMap;
	};



	template<typename T>
	struct ScriptFieldInfoProxy {
		std::string fieldName;
		std::string type;
		T data;
	};


	struct ProxyScript {
		std::string mScriptName;
		std::vector<rttr::variant> ScriptFieldProxyList;

	};


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
	MonoArray* GetMonoArray<unsigned>(std::shared_ptr<MonoDomain> md, size_t sz);

	template <>
	MonoArray* GetMonoArray<float>(std::shared_ptr<MonoDomain> md, size_t sz);

	template <>
	MonoArray* GetMonoArray<double>(std::shared_ptr<MonoDomain> md, size_t sz);

	template <>
	MonoArray* GetMonoArray<double>(std::shared_ptr<MonoDomain> md, size_t sz);

}
