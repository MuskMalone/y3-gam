#pragma once
/*!*********************************************************************
\file   ScriptInstance.h
\author han.q\@digipen.edu
\date   28-September-2023
\brief
	Script Class to contain the data of a c# class.
	It will store the pointer to the MonoObject and its 4 main methods

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/object.h>
#include <string>
#include <iostream>
#include <fstream>
#include <rttr/type.h>
#include "ScriptUtils.h"
#include "Core/Entity.h"



namespace Mono {

	const size_t maxBufferSize{ 1000 };
	enum CtorType
	{
		DEFAULT_CTOR,
		ENTITY_CTOR,
		SPECIAL_CTOR
	};

	/*!**********************************************************************
	* This struct contains the instance of a C# class field. The actual     *
	* data of the c# class field is stored in the struct                  	*
	************************************************************************/
	template<typename T>
	struct ScriptFieldInstance
	{
			ScriptFieldInstance() : mScriptField{}, mData{} { mType = rttr::variant(*this).get_type().get_name().to_string(); }
			ScriptFieldInstance(ScriptFieldInfo const& scriptField) : mScriptField{ scriptField }, mData{  } {}
			ScriptFieldInstance(ScriptFieldInfo const& scriptField, T data) : mScriptField{ scriptField }, mData{ data }
			{
				mType = rttr::variant(*this).get_type().get_name().to_string();
			}

			std::string mType;
			ScriptFieldInfo mScriptField;
			T mData;
	};



	struct ScriptInstance
	{
		CtorType mCtorType;
		ECS::Entity::EntityID mEntityID;
		std::string mScriptName;
		uint32_t mGcHandle;
		MonoClass* mScriptClass{ nullptr };   //I didn't use shared ptr for these 2 ptrs because Mono frees this memory by itself behind the scene, using its own function (Which we do not have access to). 
		MonoObject* mClassInst{ nullptr };	  //If i were to put this in a shared ptr, it will cause an error as shared ptr will try to delete the mono ptr, which is not allowed. we need to use mono's own function to delete it,but we do not have access to it
		MonoMethod* mOnCreateMethod = { nullptr };
		MonoMethod* mOnUpdateMethod = { nullptr };
		std::vector<rttr::variant> mScriptFieldInstList;
		inline static char mFieldValBuffer[maxBufferSize];

		/*!*********************************************************************
		\brief
			Default constructor of Script Class
		************************************************************************/
		ScriptInstance() {  }


		/*!*********************************************************************
		\brief
			Non default constructor of Script Class.

		\params const std::string& scriptName
			name of the script class

		\params  std::vector<void*>& arg
		arguments to pass into the scrip class's non-default constructor
		************************************************************************/
		ScriptInstance(const std::string& scriptName, std::vector<void*>& arg, bool isSpecial = false);


		/*!*********************************************************************
		\brief
			Function to hot reload the c# script
		************************************************************************/
		void ReloadScript();

		/*!*********************************************************************
		\brief
			Function to get all the c# script class public field data from ScriptManager
		************************************************************************/
		void GetAllFieldsInst();


		/*!*********************************************************************
		\brief
			Function to print out the info of all the field in the c# class. for 
			debug purposes
		************************************************************************/
		void PrintAllField();


		/*!*********************************************************************
		\brief
			Function to invoke the OnCreate method of the c# script class
		************************************************************************/
		void InvokeOnCreate();

		/*!*********************************************************************
		\brief
			Function to invoke the OnUpdate method of the c# script class
		\param dt
			Delta time
		************************************************************************/
		void InvokeOnUpdate(double dt);

		/*!*********************************************************************
		\brief
			Function to pass in the entity Id into the c# class
		\param entityId
			Id of the script's owner
		************************************************************************/
		void SetEntityID(ECS::Entity::EntityID entityId);

		/*!*********************************************************************
			\brief
				Function to clear all the data tied to the Script
			************************************************************************/
		void FreeScript();

		/*!*********************************************************************
		\brief
			Function to update all the script field instance with the values inside the c# script
			THis function is called to ensure the value displayed in the inspector is updated (in case the
			values are changed inside the c# script)
		************************************************************************/
		void GetAllUpdatedFields();


		/*!*********************************************************************
		\brief
			Function to update all the class field instance in c# script with the value inside c++
			THis function is called to ensure the value displayed in the inspector is updated into the c# script
		************************************************************************/
		void SetAllFields();







		/*!*********************************************************************
		*																																		   *
		*					Template function to Get/Set C# class field values					 *
		*																																			 *
		************************************************************************/


		/*!*********************************************************************
		\brief
			Template Function to get a public field from the c# script class

		\param MonoClassField* field
		shared pointer to the field we are trying to get

		\return
		the value of the c# script class's public field
		************************************************************************/
		template<typename T>
		T GetFieldValue(MonoClassField* field)
		{
			mono_field_get_value(mClassInst, field, mFieldValBuffer);
			return *(T*)mFieldValBuffer;
		}


		/*!*********************************************************************
		\brief
			Template Function to get a public field from the c# script class.
			this function will be called when we want get the value of a script class
			public field thats an array

		\param MonoClassField* field
		shared pointer to the field we are trying to get

		\return
		the value of the c# script class's public field
		************************************************************************/
		template<typename T>
		std::vector<T> GetFieldValueArr(MonoClassField* field)
		{
			MonoArray* newArray{};

			mono_field_get_value(mClassInst, field, &newArray);
			std::vector<T> test{};
			for (int i = 0; i < mono_array_length(newArray); ++i) {
				T element = mono_array_get(newArray, T, i);
				test.push_back(element);
			}

			return test;
		}


		/*!*********************************************************************
		\brief
			Template Function to set a public field from the c# script class

		\param T value
		Value we are trying to set with

		\param MonoClassField* field
		shared pointer to the field we are trying to get
		************************************************************************/
		template<typename T>
		void SetFieldValue(T value, MonoClassField* field)
		{
			std::memcpy(mFieldValBuffer, &value, sizeof(T));
			mono_field_set_value(mClassInst, field, mFieldValBuffer);
		}


		/*!*********************************************************************
		\brief
			Template Function to set a public field from the c# script class.
			this function will be called when we want set the value of a script class
			public field thats an array

		\param std::vector<T> value
		Value we want to set

		\param MonoClassField* field
		shared pointer to the field we are trying to get
		************************************************************************/
		template<typename T>
		void SetFieldValueArr(std::vector<T> value,MonoClassField* field, std::shared_ptr<MonoDomain> md)
		{
			MonoArray* newArray = GetMonoArray<T>(md, value.size());
			for (int i = 0; i < mono_array_length(newArray); ++i) {
				mono_array_set(newArray, T, i, value[i]);
			}
			mono_field_set_value(mClassInst, field, newArray);
		}


		/*!*********************************************************************
		\brief
			Template Function to get a public field from the c# script class.
			this function will be called when we want get the value of a data member that happens to be another script class
			public field thats an array

		\param std::shared_ptr<MonoObject>  obj
		shared pointer to the mono oject we are accessing

		\param MonoClassField* field
		shared pointer to the field we are trying to get

		\return
		the value of the c# script class's public field
		************************************************************************/
		template<typename T>
		std::vector<T> GetChildFieldValueArr(std::shared_ptr<MonoObject> obj, MonoClassField* field)
		{
			MonoArray* newArray{};
			mono_field_get_value(obj.get(), field.get(), &newArray);
			std::vector<T> test{};
			for (int i = 0; i < mono_array_length(newArray); ++i) {
				T element = mono_array_get(newArray, T, i);
				test.push_back(element);
			}

			return test;
		}

		/*!*********************************************************************
		\brief
			Template Function to set a public field from the c# script class.
			this function will be called when we want set the value of a data
			member that happens to be another script class public field thats
			an array

		\param std::shared_ptr<MonoObject>  obj
		shared pointer to the mono oject we are accessing

		\param MonoClassField* field
		shared pointer to the field we are trying to get

		\param std::vector<T> value
		Value we want to set
		************************************************************************/
		template<typename T>
		void SetChildFieldValueArr(std::shared_ptr<MonoObject> obj, MonoClassField* field, std::vector<T> value, std::shared_ptr<MonoDomain> md)
		{
			MonoArray* newArray = GetMonoArray<T>(md, value.size());
			for (int i = 0; i < mono_array_length(newArray); ++i) {
				mono_array_set(newArray, T, i, value[i]);
			}
			mono_field_set_value(obj.get(), field.get(), newArray);
		}


	};

}

