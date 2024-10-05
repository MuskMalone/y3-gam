/*!*********************************************************************
\file   ScriptManager.h
\author han.q@digipen.edu
\date   28-September-2024
\brief
	Script Manager Singleton in charge of initializing and cleaning the Mono.
	Provides function to retrieve C# class data
	Adds internal call into mono to allow C# to call functions defined in cpp

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/#pragma once
#pragma once
#include <Singleton.h>
#include <mono/jit/jit.h>
#include "mono/metadata/assembly.h"
#include "mono/metadata/object.h"
#include "mono/metadata/tabledefs.h"
#include "mono/metadata/mono-debug.h"
#include "mono/metadata/threads.h"
#include "FileWatch.h"
#include <Scripting/ScriptInstance.h>

#include <string>
#include <memory>
#include <map>
#include <unordered_map>
#include <vector>


namespace Mono
{



	class ScriptManager : public Singleton<ScriptManager>
	{
		static std::map<std::string, ScriptClassInfo> mMonoClassMap;

	public:
		static std::unordered_map<std::string, ScriptFieldType> mScriptFieldTypeMap;
		static std::vector<std::string> mAllScriptNames;
		static std::shared_ptr<MonoDomain> mRootDomain;
		static std::shared_ptr<MonoDomain> mAppDomain;
		static std::string mAppDomFilePath;
		static std::string mCoreAssFilePath;
		static std::unique_ptr<filewatch::FileWatch<std::string>> mFileWatcher;
		static bool mAssemblyReloadPending;
		static std::unique_ptr<filewatch::FileWatch<std::string>> mCsProjWatcher;
		static bool mCSReloadPending;
		static bool mRebuildCS;
		static std::string mScnfilePath;
		static std::string mCsprojPath;
		static std::string mBatfilePath;


		/*!*********************************************************************
		\brief
			destructor of Script Class. Calls mono's cleanup function to free
			the memory and shutdown mono
		************************************************************************/
		~ScriptManager();


		/*!**********************************************************************
		*																																			  *
		*								    Functions for initializing Mono											*
		*																																			  *
		************************************************************************/

		/*!*********************************************************************
		\brief
			Function to load mono app domain
		************************************************************************/
		static void LoadAppDomain();

		/*!*********************************************************************
		\brief
			Init function for Mono. Sets the assembly path, initializes the
			domains and Load the C# Assembly. Internal calls are added into mono
		************************************************************************/
		void InitMono();

		/*!*********************************************************************
		\brief
			This function is called during ScriptManager Init to create all the MonoClass*
			By having a list of all the pre-made MonoClass, user only needs to pass in the name of the
			script when they want to create an instance of a script

		\params std::ifstream& ifs
			Ifs stream to a text file containing all the script names and their namespaces
		************************************************************************/
		static void LoadAllMonoClass();

		/*!*********************************************************************
		\brief
			Function to add internal function calls to mono
		************************************************************************/
		static void AddInternalCalls();

		/*!*********************************************************************
		\brief
			Function to pass into the file watcher. detects for any changes in the mono dll file.
			If changes detected, it will reload the assembly and the c# scripts


		\params const std::string& path
			class Name of the c# class object we want to instantiate

		\params  const filewatch::Event change_type
			The change type of the file its watching (e.g modified, add)

		************************************************************************/
		static void AssemblyFileSystemEvent(const std::string& path, const filewatch::Event change_type);


		

		/*!**********************************************************************
		*																																			  *
		*					     Helper functions used by Script Instances								*
		*																																			  *
		************************************************************************/


		/*!*********************************************************************
		\brief
			Function to create a C# class instance by calling its non-default
			constructor and passing in the relevant arguments

		\params const char* className
			class Name of the c# class object we want to instantiate

		\params std::vector<void*>& arg
			arguments to pass into the class's non-default constructor

			\return
			Instance of the c# class in the form of MonoObject*
		************************************************************************/
		MonoObject* InstantiateClass(const char* className, std::vector<void*>& arg);

		/*!*********************************************************************
		\brief
			Function to get A MonoClass* from the ScriptManager

		\params std::string className
			name of the class

		\return
			MonoClass* obejct that will be used to generate and instance of a c# class
		************************************************************************/
		std::shared_ptr<MonoClass> GetScriptClass(std::string className);

		/*!*********************************************************************
		\brief
			Function to get the script class info of  c# class

		\params std::string className
			name of the class

		\return
			A scriptClassInfo object that contains all the details of a c# class's public data member
		************************************************************************/
		ScriptClassInfo GetScriptClassInfo(std::string className);

		/*!*********************************************************************
		\brief
			Load the C# Assembly Data from the DLL file

		\params assemblyPath
			path to the C# DLL file
		************************************************************************/
		ScriptFieldInfo GetScriptField(std::string className, std::string fieldName);

		/*!*********************************************************************
		\brief
			Function to convert a monotype to my ScriptFieldType enum

		\params MonoType* monoType
		C# mono type

		\return
		A ScriptFieldTYpe enum representing the monotype
		************************************************************************/
		static ScriptFieldType MonoTypeToScriptFieldType(MonoType* monoType);



		/*!**********************************************************************
		*																																			  *
		*					          Helper functions for hot reload											*
		*																																			  *
		************************************************************************/


		/*!*********************************************************************
		\brief
			Function to reload all the c# scripts from mono
		************************************************************************/
		static void ReloadAllScripts();

		/*!*********************************************************************
		\brief
			Function to pass into the file watcher. detects for any changes in the c# project code.
			If changes detected, it will rebuild the c# project to generate a new dll file


		\params const std::string& path
			class Name of the c# class object we want to instantiate

		\params  const filewatch::Event change_type
			The change type of the file its watching (e.g modified, add)
		************************************************************************/
		static void CSReloadEvent(const std::string& path, const filewatch::Event change_type);

		static void ReloadAssembly();

		static void ReloadScripts();

		static void RebuildCS();

	};

		/*!**********************************************************************
		*																																			  *
		*								            Internal Calls			          						  *
		*																																			  *
		************************************************************************/


		/*!*********************************************************************
		\brief
			Function to set the world position of an entity's transform component.
			This function will be added as internal call to allow c# script to set
			entities' transform.
		\params entity
			ID of the entity

		\params transformAdjustment
			values to be added to the entity's transform
		************************************************************************/
		static void SetWorldPosition(ECS::Entity::EntityID entity, glm::vec3 posAdjustment);

		/*!*********************************************************************
		\brief
			Set the scale of the entity
		\param GE::ECS::Entity entity
			ID of the entity
		\param GE::Math::dVec3 PosAdjustment
			Vector 3 of the new scale
		************************************************************************/
		static void SetWorldScale(ECS::Entity::EntityID entity, glm::vec3 scaleAdjustment);

		/*!*********************************************************************
		\brief
			Set the rotation of the entity
		\param GE::ECS::Entity entity
			ID of the entity
		\param GE::Math::dVec3 PosAdjustment
			Vector 3 of the new rotation
		************************************************************************/
		static void SetRotation(ECS::Entity::EntityID entity, glm::vec3 rotAdjustment);

		/*!*********************************************************************
			\brief
				Get the world position of the entity
			\param GE::ECS::Entity entity
				ID of the entity
			\return GE::Math::dVec3
				Returns a vector 3 of the position of the entity
			************************************************************************/
		static glm::vec3 GetWorldPosition(ECS::Entity::EntityID entity);

		/*!*********************************************************************
		\brief
			Get the scale of the entity
		\param GE::ECS::Entity entity
			ID of the entity
		\return GE::Math::dVec3
			Returns a vector 3 of the scale of the entity
		************************************************************************/
		static glm::vec3 GetWorldScale(ECS::Entity::EntityID entity);

		/*!*********************************************************************
		\brief
			Get the rotation of the entity
		\param GE::ECS::Entity entity
			ID of the entity
		\return GE::Math::dVec3
			Returns a vector 3 of the rotation of the entity
		************************************************************************/
		static glm::vec3 GetRotation(ECS::Entity::EntityID entity);

	/*!*********************************************************************
		\brief
			Function to read data from a file and store it in a char buffer

		\params filepath
			path of the file

		\params outSize
			size of the file

		\return
			char buffer containing the data of the file
		************************************************************************/
	char* ReadBytes(const std::string& filepath, uint32_t* outSize);


	/*!*********************************************************************
		\brief
			This function returns the data of c# class.
			The class data will be used to generate instances in other parts of the script manager

		\params assembly
			pointer to the C# Assembly data

		\params namespaceName
		Namespace that the c# belongs in

		\params className
		Name of the c# class
		************************************************************************/
		MonoClass* GetClassInAssembly(MonoAssembly* assembly, const char* namespaceName, const char* className);


		/*!*********************************************************************
		\brief
			Load the C# Assembly Data from the DLL file

		\params assemblyPath
			path to the C# DLL file
		************************************************************************/
			MonoAssembly* LoadCSharpAssembly(const std::string& assemblyPath);

			/*!*********************************************************************
			\brief
				Function to check if a monostring is valid
			\param MonoError& error
			MonoError belonging to a MonoString
			\return
			bool alue to indicate if the Monostring is valid
			************************************************************************/
			bool CheckMonoError(MonoError& error);

		/*!*********************************************************************
		\brief
			Function to convert a C# MonoString to C++ std::string
		\param MonoString* str
		C# Monostring we want to convert
		\return
		The converted std::string
		************************************************************************/
		std::string MonoStringToSTD(MonoString* str);

		/*!*********************************************************************
		\brief
			Function to convert a std::string to C# MonoString
		\param str
			String to convert
		\return
			The converted MonoString
		************************************************************************/
		MonoString* STDToMonoString(const std::string& str);



		/*!**********************************************************************
		*																																			  *
		*												Function for debugging													*
		*																																			  *
		************************************************************************/
		void TestReload();
}
