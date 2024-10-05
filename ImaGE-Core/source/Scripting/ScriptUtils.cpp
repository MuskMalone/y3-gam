/*!*********************************************************************
\file   ScriptUtils.cpp
\author han.q\@digipen.edu
\date   26 November 2023
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
	MonoArray* GetMonoArray(std::shared_ptr<MonoDomain> md, uintptr_t sz) {
		return mono_array_new(md.get(), mono_get_int32_class(), sz);
	}

	template <>
	MonoArray* GetMonoArray<int>(std::shared_ptr<MonoDomain> md, uintptr_t sz) {
		return mono_array_new(md.get(), mono_get_int32_class(), sz);
	}

	template <>
	MonoArray* GetMonoArray<unsigned>(std::shared_ptr<MonoDomain> md, uintptr_t sz) {
		return mono_array_new(md.get(), mono_get_uint32_class(), sz);
	}

	template <>
	MonoArray* GetMonoArray<float>(std::shared_ptr<MonoDomain> md, uintptr_t sz) {
		return mono_array_new(md.get(), mono_get_single_class(), sz);
	}

	template <>
	MonoArray* GetMonoArray<double>(std::shared_ptr<MonoDomain> md, uintptr_t sz) {
		return mono_array_new(md.get(), mono_get_double_class(), sz);
	}

	template <>
	MonoArray* GetMonoArray<MonoString*>(std::shared_ptr<MonoDomain> md, size_t sz) {
		return mono_array_new(md.get(), mono_get_string_class(), sz);
	}
}