/******************************************************************************/
/*!
\file     InternalCalls.cs

\author   Han Qin Ding(han.q@digipen.edu), Ernest Cheo (e.cheo@digipen.edu)
\date     28 Septmeber 2024

\brief      
  All C# internal calls go here, where information from CPP code 
  can be accessed in C#, and vice versa

Copyright (C) 2023 DigiPen Institute of Technology. Reproduction
or disclosure of this file or its contents with the prior written 
consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.CompilerServices;
using System.Numerics;
using static System.Runtime.CompilerServices.RuntimeHelpers;

namespace IGE.Utils
{
  public static class InternalCalls
  {
    #region Transform

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static Vector3 GetScale(uint ID);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void SetScale(uint entityHandle, ref Vector3 scale);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static Vector3 GetColliderScale(uint ID);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static Vector3 GetWorldPosition(uint ID);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static Vector3 GetPosition(uint ID);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void SetPosition(uint entityHandle, ref Vector3 position);

    // avoid using this, prefer SetPosition (local)
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void SetWorldPosition(uint entityHandle, ref Vector3 position);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static Quaternion GetRotation(uint ID);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static Vector3 GetRotationEuler(uint ID);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void SetRotationEuler(uint ID, ref Vector3 position);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static Vector3 GetWorldRotationEuler(uint ID);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void SetRotation(uint entityHandle, ref Quaternion position);


    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static Quaternion GetWorldRotation(uint ID);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void SetWorldRotation(uint entityHandle, ref Quaternion position);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static Vector3 GetVelocity(uint ID);
    #endregion


    #region Input
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static bool IsKeyTriggered(KeyCode key);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static bool IsKeyHeld(KeyCode key);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static bool IsKeyPressed(KeyCode key);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static float GetAxis(string s);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static bool AnyKeyDown();

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static bool AnyKeyTriggered();

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static Vector3 GetMouseDelta();

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static Vector3 GetMousePos();

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static float GetScreenWidth();

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void SetBrightness(float fraction);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static float GetGammaNorm();

  [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static Vector3 GetMousePosWorld(float depth);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static Vector3 GetCameraForward();

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static Vector3 GetCameraRight();

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static string GetInputString();

    #endregion


    #region Logging
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static void Log(string s);


    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static void LogWarning(string s);


    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static void LogError(string s);


    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static void LogCritical(string s);
    #endregion


    #region Entity

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static string GetTag(uint EntityID);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static void SetTag(uint EntityID, string tag); // Try not to use, dangerous

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static void SetActive(uint entityHandle, bool active);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static bool IsActive(uint entityHandle);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static void DestroyEntity(uint EntityID);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static void DestroyScript(Entity obj, uint EntityID);


    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static Entity FindScriptInEntity(uint entity,string s);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static uint FindChildByTag(uint EntityID, string s);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static uint[] GetAllChildren(uint ID);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static uint FindParentByTag(string s);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static void UnparentEntity(uint entityID);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static uint GetParentByID(uint EntityID);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static Vector3 GetMainCameraPosition(uint cameraEntityID);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static Vector3 GetMainCameraDirection(uint cameraEntityID);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static Quaternion GetMainCameraRotation(uint cameraEntityID);

    #endregion


    #region Text Component

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static Vector4 GetTextColor(uint EntityID);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static void SetTextColor(uint TextEntityID, Vector4 textColor);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static float GetTextScale(uint EntityID);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static void SetTextScale(uint TextEntityID, float textScale);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static string GetText(uint EntityID);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static void SetText(uint TextEntityID, string textContent);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static void AppendText(uint TextEntityID, string textContent);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static void SetTextFont(uint TextEntityID, string textFontName);
    #endregion


    #region Color Component

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static Vector4 GetImageColor(uint EntityID);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static void SetImageColor(uint EntityID, Vector4 val);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static Vector4 GetSprite2DColor(uint EntityID);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static void SetSprite2DColor(uint EntityID, Vector4 val);
    #endregion


    #region Light Component
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static float GetLightIntensity(uint entityID);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static void SetLightIntensity(uint entityID, float intensity);
    #endregion


    #region Bloom Component
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static float GetBloomIntensity(uint entityID);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static void SetBloomIntensity(uint entityID, float intensity);
    #endregion

    #region Canvas Component
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static void SetCanvasTransitionProgress(uint entityID, float intensity);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static void EnableCanvasTransition(uint entityID, bool isEnabled);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static void SetCanvasTransitionType(uint entityID, int intensity);
    #endregion

    #region Utility


    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static string GetLayerName(uint entity);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static uint Raycast(Vector3 start, Vector3 end);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static uint RaycastFromEntity(uint entity, Vector3 start, Vector3 end);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static RaycastHitInfo RaycastFromEntityInfo(uint entity, Vector3 start, Vector3 end);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static void SetSoundPitch(uint entity, string sound, float pitch);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static void SetSoundVolume(uint entity, string sound, float volume);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static void SetSoundGlobalVolume(float volume);


    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static void EnableSoundPostProcessing(uint entity, string sound, uint type, float param);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static void DisableSoundPostProcessing(uint entity, string sound);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static void PlaySound(uint entity, string sound);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static void PauseSound(uint entity, string sound);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static void StopSound(uint entity, string sound);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static void PlaySoundFromPosition(uint entity, string sound);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static void GetSoundPlaybackPosition(uint entity, string sound, uint time);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static void PlayAnimation(uint entity, string animation, bool loop = false);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static bool IsPlayingAnimation(uint entity);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static string GetCurrentAnimation(uint entity);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static void PauseAnimation(uint entity);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static void ResumeAnimation(uint entity);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static void StopAnimationLoop(uint entity); // stops the animation after the current loop ends

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static float GetDeltaTime();

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static float GetTime();

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static float GetFPS();

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static void MoveCharacter(uint mEntityID, Vector3 dVec);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static void SetAngularVelocity(uint mEntityID, Vector3 angularVelocity);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static void SetVelocity(uint mEntityID, Vector3 velocity);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static float GetGravityFactor(uint mEntityID);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static void SetGravityFactor(uint mEntityID, float gravityFactor);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    // updates physics of the entity to align with its world transform values
    internal extern static void UpdatePhysicsToTransform(uint entity);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void LockRigidBody(uint entity, bool toLock);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void LockRigidBodyRotation(uint entity, bool x, bool y, bool z);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static Entity FindScript(string s);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static string GetCurrentScene();

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static void SetCurrentScene(string sceneName);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static void TakeScreenShot(string s, int width, int height);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static bool SetDaySkyBox(uint mEntityID, float speed);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static void ShowCursor();

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static void HideCursor();

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static bool OnTriggerEnter(uint entityTrigger, uint entityOther);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static bool OnTriggerExit(uint entityTrigger, uint entityOther);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static ContactPoint[] GetContactPoints(uint entity1, uint entity2);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static float GetShortestDistance(uint e1, uint e2);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static void ChangeToolsPainting();

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static void SpawnToolBox();

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static void SpawnOpenDoor();

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static void SpawnTaraSilhouette();

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static void SetShaderState(uint idx, bool active);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static void PauseGame();

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static void ResumeGame();

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    extern public static bool GetIsPaused();
    #endregion
  }
}
