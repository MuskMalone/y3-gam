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
#include <Singleton/ThreadSafeSingleton.h>
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

#define IGE_SCRIPTMGR Mono::ScriptManager::GetInstance()

namespace Mono
{
	class ScriptManager : public ThreadSafeSingleton<ScriptManager>
	{
		static std::map<std::string, ScriptClassInfo> mMonoClassMap;  //Map of Scripts 
	private:
		ScriptInstance mEntityBaseTemplate;
		
	public:
		static std::unordered_map<std::string, ScriptFieldType> mScriptFieldTypeMap;
		static std::unordered_map<ScriptFieldType, std::string> mRevClassMap;			//Rev Map of Scripts, for getting the name of script based on type 
		static std::vector < std::tuple<std::string, int, int>>mScreenShotInfo;
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
		static bool mTriggerStart;


		/*!*********************************************************************
		\brief
			Init function for Mono. Sets the assembly path, initializes the
			domains and Load the C# Assembly. Internal calls are added into mono
		************************************************************************/
		ScriptManager();

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


		static bool IsMonoBehaviourclass(MonoClass* mc);


		static void LinkAllScriptDataMember();


		

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
		MonoObject* InstantiateClass(const char* className, std::vector<void*> arg = std::vector<void*>());

		/*!*********************************************************************
		\brief
			Function to get A MonoClass* from the ScriptManager

		\params std::string className
			name of the class

		\return
			MonoClass* obejct that will be used to generate and instance of a c# class
		************************************************************************/
		MonoClass* GetScriptClass(std::string className);

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


		std::vector<ScriptInstance> SerialMonoObjectVec(std::vector<MonoObject*> const& vec);


		static void TriggerStart();


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
			Function to set the world/local position of an entity's transform component.
			This function will be added as internal call to allow c# script to set
			entities' transform.
		\params entity
			ID of the entity

		\params transformAdjustment
			values to be added to the entity's transform
		************************************************************************/
		static void SetWorldPosition(ECS::Entity::EntityID entity, glm::vec3 posAdjustment);
		static void SetPosition(ECS::Entity::EntityID entity, glm::vec3 posAdjustment);

		/*!*********************************************************************
		\brief
			Set the scale of the entity
		\param GE::ECS::Entity entity
			ID of the entity
		\param GE::Math::dVec3 PosAdjustment
			Vector 3 of the new scale
		************************************************************************/
		static void SetWorldScale(ECS::Entity::EntityID entity, glm::vec3 scaleAdjustment);
		static glm::vec3 GetScale(ECS::Entity::EntityID);
		static void SetScale(ECS::Entity::EntityID entity, glm::vec3 scale);
		static glm::vec3 GetColliderScale(ECS::Entity::EntityID);

		/*!*********************************************************************
		\brief
			Set the rotation of the entity
		\param GE::ECS::Entity entity
			ID of the entity
		\param GE::Math::dVec3 PosAdjustment
			Vector 3 of the new rotation
		************************************************************************/
		static glm::quat GetWorldRotation(ECS::Entity::EntityID entity);
		static void SetWorldRotation(ECS::Entity::EntityID entity, glm::quat rotAdjustment);
		static glm::quat GetRotation(ECS::Entity::EntityID entity);
		static void SetRotation(ECS::Entity::EntityID entity, glm::quat rotAdjustment);
		static glm::vec3 GetWorldRotationEuler(ECS::Entity::EntityID entity);
		static glm::vec3 GetRotationEuler(ECS::Entity::EntityID entity);
		static void SetRotationEuler(ECS::Entity::EntityID entity, glm::vec3 rotAdjustment);


		/*!*********************************************************************
			\brief
				Get the world/local position of the entity
			\param GE::ECS::Entity entity
				ID of the entity
			\return GE::Math::dVec3
				Returns a vector 3 of the position of the entity
			************************************************************************/
		static glm::vec3 GetWorldPosition(ECS::Entity::EntityID entity);
		static glm::vec3 GetPosition(ECS::Entity::EntityID entity);

		/*!*********************************************************************
		\brief
			Get the scale of the entity
		\param GE::ECS::Entity entity
			ID of the entity
		\return GE::Math::dVec3
			Returns a vector 3 of the scale of the entity
		************************************************************************/
		static glm::vec3 GetWorldScale(ECS::Entity::EntityID entity);


		static MonoString* GetTag(ECS::Entity::EntityID entity);
		static void SetTag(ECS::Entity::EntityID entity, MonoString* tag);

		static void Log(MonoString*);

		static void LogWarning(MonoString*);

		static void LogError(MonoString*);

		static void LogCritical(MonoString*);

		static float GetAxis(MonoString*);

		static void MoveCharacter(ECS::Entity::EntityID entity, glm::vec3 dVec);

		static void SetAngularVelocity(ECS::Entity::EntityID entity, glm::vec3 angularVelocity);

		static void SetVelocity(ECS::Entity::EntityID entity, glm::vec3 velocity);

		static float GetGravityFactor(ECS::Entity::EntityID entity);
		static void SetGravityFactor(ECS::Entity::EntityID entity, float gravity);

		static ECS::Entity::EntityID Raycast(glm::vec3 start, glm::vec3 end);

		static ECS::Entity::EntityID RaycastFromEntity(ECS::Entity::EntityID e, glm::vec3 start, glm::vec3 end);

		static void SetSoundPitch(ECS::Entity::EntityID, MonoString*, float);
		static void SetSoundVolume(ECS::Entity::EntityID, MonoString*, float);
		static void PlaySound(ECS::Entity::EntityID, MonoString*);
		static void PauseSound(ECS::Entity::EntityID, MonoString*);
		static void StopSound(ECS::Entity::EntityID, MonoString*);

		static void PlayAnimation(ECS::Entity::EntityID entity, MonoString* name, bool loop);
		static bool IsPlayingAnimation(ECS::Entity::EntityID entity);
		static void PauseAnimation(ECS::Entity::EntityID entity);
		static void ResumeAnimation(ECS::Entity::EntityID entity);
		static void StopAnimationLoop(ECS::Entity::EntityID entity);

		// updates physics of the entity to align with its world transform values
		static void UpdatePhysicsToTransform(ECS::Entity::EntityID entity);

		static glm::vec3 GetVelocity(ECS::Entity::EntityID);

		static MonoString* GetLayerName(ECS::Entity::EntityID);
		
		static float GetDeltaTime();

		static float GetTime();

		static float GetFPS();

		static glm::vec3 GetMouseDelta();

		static MonoObject* FindScript(MonoString* s);   //Returns the first script it can find from all the entities

		static MonoObject* FindScriptInEntity(ECS::Entity::EntityID entity, MonoString* s); //Return a script that belongs to an entity

		static void DestroyEntity(ECS::Entity::EntityID entity);

		static void DestroyScript(MonoObject* obj, ECS::Entity::EntityID entity);

		static void SetActive(ECS::Entity::EntityID entity,bool b);

		static bool IsActive(ECS::Entity::EntityID entity);

		static glm::vec3 GetMousePos();

		static glm::vec3 GetMousePosWorld(float depth);

		static glm::vec3 GetCameraForward();

		static ECS::Entity::EntityID FindChildByTag(ECS::Entity::EntityID entity, MonoString* s);

		static ECS::Entity::EntityID FindParentByTag(MonoString* s);

		static ECS::Entity::EntityID GetParentByID(ECS::Entity::EntityID entity);

		static MonoArray* GetAllChildren(ECS::Entity::EntityID entity);

		static glm::vec3 GetMainCameraPosition(ECS::Entity::EntityID cameraEntity);

		static glm::vec3 GetMainCameraDirection(ECS::Entity::EntityID cameraEntity);

		static glm::quat GetMainCameraRotation(ECS::Entity::EntityID cameraEntity);
		
		static glm::vec4 GetTextColor(ECS::Entity::EntityID textEntity);

		static void SetTextColor(ECS::Entity::EntityID textEntity, glm::vec4 textColor);

		static float GetTextScale(ECS::Entity::EntityID textEntity);

		static void SetTextScale(ECS::Entity::EntityID textEntity, float textScale);

		static MonoString* GetText(ECS::Entity::EntityID entity);

		static void SetText(ECS::Entity::EntityID textEntity, MonoString* textContent);

		static void AppendText(ECS::Entity::EntityID textEntity, MonoString* textContent);

		static void SetTextFont(ECS::Entity::EntityID textEntity, MonoString* s);

		static glm::vec4 GetImageColor(ECS::Entity::EntityID entity);

		static void SetImageColor(ECS::Entity::EntityID entity, glm::vec4 val);

		static glm::vec4 GetSprite2DColor(ECS::Entity::EntityID entity);
		static void SetSprite2DColor(ECS::Entity::EntityID entity, glm::vec4 val);
		
		static MonoString* GetCurrentScene();
		static void SetCurrentScene(MonoString* scenePath);

		static void TakeScreenShot(MonoString* name, int width, int height);
		void SaveScreenShot(std::string name, int width, int height);

		static bool SetDaySkyBox(ECS::Entity::EntityID cameraEntity,float speed);

		static void ShowCursor();

		static void HideCursor();

		static void ChangeToolsPainting();
		

		static void SpawnToolBox();

		static void SpawnOpenDoor();

		static bool OnTriggerEnter(ECS::Entity trigger, ECS::Entity other);

		static bool OnTriggerExit(ECS::Entity trigger, ECS::Entity other);

		/*!**********************************************************************
		*																																			  *
		*								            Helper Function			          						  *
		*																																			  *
		************************************************************************/
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

		static float* Test(ECS::Entity::EntityID entity);

		/*!**********************************************************************
		*																																			  *
		*												Function for debugging													*
		*																																			  *
		************************************************************************/
		void TestReload();
}
