/*!*********************************************************************
\file   ScriptUtils.cpp
\author han.q\@digipen.edu
\date   5 October 2024
\brief
	This file contains the definition for utils function required to
	transfer data between C# and C++

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#pragma once
#include <pch.h>
#include <Scripting/ScriptUtils.h>
#include <iostream>

namespace Mono
{
	template <typename T>
	MonoArray* GetMonoArray(MonoDomain* md, uintptr_t sz) {
		return mono_array_new(md, mono_get_int32_class(), sz);
	}

	template <>
	MonoArray* GetMonoArray<int>(MonoDomain* md, uintptr_t sz) {
		return mono_array_new(md, mono_get_int32_class(), sz);
	}


	template <>
	MonoArray* GetMonoArray<std::string>(MonoDomain* md, uintptr_t sz) {
		return mono_array_new(md, mono_get_string_class(), sz);
	}

	template <>
	MonoArray* GetMonoArray<unsigned>(MonoDomain* md, uintptr_t sz) {
		return mono_array_new(md, mono_get_uint32_class(), sz);
	}

	template <>
	MonoArray* GetMonoArray<float>(MonoDomain* md, uintptr_t sz) {
		return mono_array_new(md, mono_get_single_class(), sz);
	}

	template <>
	MonoArray* GetMonoArray<double>(MonoDomain* md, uintptr_t sz) {
		return mono_array_new(md, mono_get_double_class(), sz);
	}

	template <>
	MonoArray* GetMonoArray<MonoString*>(MonoDomain* md, size_t sz) {
		return mono_array_new(md, mono_get_string_class(), sz);
	}

	template <>
	MonoArray* GetMonoArray<MonoObject*>(MonoDomain* md, size_t sz) {
		return mono_array_new(md, mono_get_object_class(), sz);
	}

}