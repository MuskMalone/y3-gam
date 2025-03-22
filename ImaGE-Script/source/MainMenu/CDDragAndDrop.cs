/******************************************************************************/
/*!
\par        Image Engine
\file       .cs

\author     
\date       

\brief      


Copyright (C) 2024 DigiPen Institute of Technology. Reproduction
or disclosure of this file or its contents without the prior
written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/


using IGE.Utils;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;
using System.Xml;
using System.Numerics;

public class CDDragAndDrop : Entity
{
       
    public Entity NewGameCDinCase;
    public Entity ContinueCDinCase;

    public Entity cdPlayer;
    //public Entity cdLidOpened;
    //public Entity cdLidClosed;
    //public Entity CDCurr;

    public Transition transition;

    private Vec3<float> originalScale;
    private Vec3<float> hoverScale;
    public Vector3 outOfTheWay = new Vector3(10.0f, 10.0f, 10.0f);

  private bool initial = true;
    private bool isFading = false;
    private float fadeDuration = 3f;
    private float fadeElapsed = 0f;

    private float startAlpha = 0f; // Initial alpha
    private float targetAlpha = 1f; // Target alpha

    private bool isHovered = false;

    //shakecd
    private bool isShaking = false;
    private float shakeDuration = 0f;
    private float shakeElapsed = 0f;
    private float shakeMagnitude = 0f;
    private Vector3 originalPosition;
    private float fakeRandomValue = 0f;

    private bool isBeingDragged = false;

    //rotation
    private Quaternion originalRotation;
    private float zRotAngle = 229.3f;

    //sound
    private bool isSpinningSoundPlaying = false;
    //private float spinningSoundTimer = 0f;

    //dragging
    private float toCDDist;
    private Vector3 toCDVec;

    //openorclose
    public bool isLidOpen = false;
    void Start()
    {
        InternalCalls.ShowCursor();
        originalPosition = InternalCalls.GetWorldPosition(mEntityID);
        originalRotation = InternalCalls.GetWorldRotation(mEntityID);
        //Console.WriteLine("originalrotation " + originalRotation);

        //cdLidClosed.SetActive(true);
        //cdLidOpened.SetActive(false);

        //Console.WriteLine("My Entity WorldPos INITIAL " + InternalCalls.GetWorldPosition(mEntityID));
        NewGameCDinCase.SetActive(false);
        ContinueCDinCase.SetActive(false);

        Vector3 scaleVector = InternalCalls.GetScale(mEntityID);
        originalScale = new Vec3<float>(scaleVector.X, scaleVector.Y, scaleVector.Z);
        hoverScale = new Vec3<float>(
            originalScale.X * 1.1f,
            originalScale.Y * 1.1f,
            originalScale.Z * 1.1f
        );
    }
        
    //private Image FindImageInChildren(uint parentEntityID)
    //{
    //    // Get all children of the parent entity
    //    uint[] childEntities = InternalCalls.GetAllChildren(parentEntityID);

    //    foreach (uint childID in childEntities)
    //    {
    //        // Check if the child has the Image script/component
    //        Entity childEntity = InternalCalls.FindScriptInEntity(childID, "Image") as Entity;
    //        if (childEntity != null)
    //        {
    //            return childEntity.GetComponent<Image>();
    //        }
    //    }

    //    return null; // Return null if no Image component is found
    //}

    void Update()
    {
        //Vector3 MousePos = InternalCalls.GetMousePosWorld(1.0f);
        //Vector3 MousPosRayEnd = MousePos + (InternalCalls.GetCameraForward() * 5.0f);

        //uint hitEntity = InternalCalls.Raycast(MousePos, MousPosRayEnd);
        //Console.WriteLine("Entity " + InternalCalls.GetTag(hitEntity) + " hit ");
        //Console.WriteLine(MousePos.ToString() + " " + MousPosRayEnd.ToString());
            //string tag = InternalCalls.GetTag(mEntityID);
        if (initial)
        {
          transition.StartTransition(true, 3f, Transition.TransitionType.TV_SWITCH);
          initial = false;
        }
        if (isHovered && !isBeingDragged)
        {
            //Console.WriteLine("Entered isHovered");
            Vector3 hoverVector = new Vector3(hoverScale.X, hoverScale.Y, hoverScale.Z);
            InternalCalls.SetScale(mEntityID, ref hoverVector);
            
            //rotation
            string tag = InternalCalls.GetTag(mEntityID);
            if (tag == "NewGameCDChild" || tag == "ContinueCDChild")
            {
                if (!isSpinningSoundPlaying)
                {
                    //Console.WriteLine("Here?");
                    InternalCalls.PlaySound(mEntityID, "SpinningDiscPS1_SFX");
                    isSpinningSoundPlaying = true;
                    //spinningSoundTimer = 0f;
                }
                zRotAngle += -2f * Time.deltaTime;
                Quaternion zRot = Quaternion.CreateFromAxisAngle(Vector3.UnitZ, zRotAngle);

                Quaternion xRotation = Quaternion.CreateFromAxisAngle(Vector3.UnitX, Mathf.DegToRad(90));

                // Combine the rotations
                Quaternion combinedRotation = zRot * xRotation; // Order matters!

                // Apply the combined rotation
                InternalCalls.SetWorldRotation(mEntityID, ref combinedRotation);
            }
            

        }
        else if(!isHovered)
        {
            Vector3 originalVector = new Vector3(originalScale.X, originalScale.Y, originalScale.Z);
            InternalCalls.SetScale(mEntityID, ref originalVector);
            
        }

        if (isBeingDragged)
        {
            
            FollowMouse();

            //try to rotate when above release area and mousedown (kinda laggy so commented out)
            //bool mouseDownOnCDPlayer = false;
            //Vector3 MousePos = InternalCalls.GetMousePosWorld(1.0f);
            //Vector3 MousPosRayEnd = MousePos + (InternalCalls.GetCameraForward() * 5.0f);
            //uint hitEntity = InternalCalls.Raycast(MousePos, MousPosRayEnd);

            //if (InternalCalls.GetTag(hitEntity) == "CDPlayer_Body")
            //{
            //    //Console.WriteLine("hit cd player");
            //    mouseDownOnCDPlayer = true;
            //    //Quaternion xRotation = Quaternion.CreateFromAxisAngle(Vector3.UnitX, Mathf.DegToRad(0));
            //    //InternalCalls.SetWorldRotation(mEntityID, ref xRotation);
            //}
            ////else
            ////{
            ////    InternalCalls.SetWorldRotation(mEntityID, ref originalRotation);
            ////}
            //if(mouseDownOnCDPlayer)
            //{
            //    Quaternion xRotation = Quaternion.CreateFromAxisAngle(Vector3.UnitX, Mathf.DegToRad(0));
            //    InternalCalls.SetWorldRotation(mEntityID, ref xRotation);
            //}
            //else
            //{
            //    InternalCalls.SetWorldRotation(mEntityID, ref originalRotation);
            //}

        }

        if (isFading)
        {
            fadeElapsed += Time.deltaTime;
            float alpha = Mathf.Lerp(startAlpha, targetAlpha, fadeElapsed / fadeDuration);
            string tag = InternalCalls.GetTag(mEntityID);

            float volume = Mathf.Lerp(1.0f, 0.0f, fadeElapsed / fadeDuration); // Fade volume from 1.0 to 0.0
            InternalCalls.SetSoundVolume(mEntityID, "MainMenuBGM", volume); // Apply the volume fade
            if (fadeElapsed >= fadeDuration)
            {
                isFading = false;
                //InternalCalls.StopSound(mEntityID, "..\\Assets\\Audio\\Picture Perfect - BGM1_Vers1_Loop.wav");
                if (tag == "NewGameCDChild")
                {
                    InternalCalls.SetCurrentScene("..\\Assets\\Scenes\\TutorialLevel.scn");
                }
                else if(tag == "ContinueCDChild")
                {
                    InternalCalls.SetCurrentScene("..\\Assets\\Scenes\\M3.scn");
                }
                //InternalCalls.SetCurrentScene("..\\Assets\\Scenes\\M3.scn");
            }
        }

        //shakingcd
        if (isShaking)
        {
            //Debug.Log("inIsShaking");
            shakeElapsed += Time.deltaTime;
            //float seed = shakeElapsed; 
            //float offsetX = ShakeRandom(seed) * shakeMagnitude;
            //float offsetY = ShakeRandom(seed + 1f) * shakeMagnitude; 

            fakeRandomValue += 0.1f; 

            float offsetX = (fakeRandomValue * 1.2f) % 2f - 1f; 
            float offsetY = (fakeRandomValue * 3.8f) % 2f - 1f;

            offsetX *= shakeMagnitude;
            offsetY *= shakeMagnitude;
            Vector3 shakingPos = new Vector3(originalPosition.X + offsetX, originalPosition.Y + offsetY, originalPosition.Z);
            InternalCalls.SetWorldPosition(mEntityID, ref shakingPos);
            //stop shake when duration is up
            if (shakeElapsed >= shakeDuration)
            {
                //Debug.Log("OvershakeDuration");
                isShaking = false;
                InternalCalls.SetWorldPosition(mEntityID, ref originalPosition);
            }

        }
    }

    float ShakeRandom(float seed)
    {
        return Mathf.Sin(seed * 1000f) % 1f; 
    }


    private void Startfade()
    {
        isFading = true;
        fadeElapsed = 0f;
        transition.StartTransition(false, fadeDuration, Transition.TransitionType.TV_SWITCH);
      //startAlpha = fadeImage != null ? fadeImage.color.a : 0f;
      //targetAlpha = 1f;
      //fadeDuration = 2.5f;
      //FadeManager.Instance.FadeToBlack(2.5f);
    }
    public void OnMouseDown()
    {
        //shaking cds
        string tag = InternalCalls.GetTag(mEntityID);
        //if(tag == "ContinueCDChild" || tag == "CreditsCDChild" || tag == "SettingsCDChild")
        if (tag == "CreditsCDChild" || tag == "SettingsCDChild")
        {
            //Debug.Log("Entered tag == for shakecd");
            
            ShakeCD(0.5f, 0.005f);
            return;
        }
        isLidOpen = true;
        isBeingDragged = true;
        //play sound
        InternalCalls.PlaySound(mEntityID, "PickupCD_SFX");

        //CDCurr.SetActive(false);

        //Startfade();


    }

    public void OnMouseUp()
    {
        isBeingDragged = false;
        bool mouseOnCDPlayer = false;
        Vector3 MousePos = InternalCalls.GetMousePosWorld(1.0f);
        Vector3 MousPosRayEnd = MousePos + (InternalCalls.GetCameraForward() * 5.0f);
        uint hitEntity = InternalCalls.Raycast(MousePos, MousPosRayEnd);
        string tag = InternalCalls.GetTag(mEntityID);
        if (InternalCalls.GetTag(hitEntity) == "CDPlayer_Body") 
        {

            mouseOnCDPlayer = true;
        }

        if (mouseOnCDPlayer)
        {
            isLidOpen = true;
            if (tag == "NewGameCDChild")
            {
                //Console.WriteLine("bitch");
                NewGameCDinCase.SetActive(true);
            }
            else if (tag == "ContinueCDChild")
            {
                ContinueCDinCase.SetActive(true);
            }
            InternalCalls.SetWorldPosition(mEntityID, ref outOfTheWay);
            NextScene();
        }
        else
        {
            isLidOpen = false;
            InternalCalls.SetWorldPosition(mEntityID, ref originalPosition);
        }
    }

    private void FollowMouse()
    {
        Vector3 MousePos = InternalCalls.GetMousePosWorld(toCDDist);
        //mouse offset to make sure the mouse is not in the cd hole
        MousePos.X += 0.03f;
        MousePos.Y += 0.03f;
        InternalCalls.SetWorldPosition(mEntityID, ref MousePos);
    }
    private void ShakeCD(float duration, float magnitude)
    {
        if (isShaking) return;

        InternalCalls.PlaySound(mEntityID, "WrongInput_SFX");
        fakeRandomValue = 0f;
        shakeDuration = duration;
        shakeMagnitude = magnitude;
        shakeElapsed = 0f;
        originalPosition = InternalCalls.GetWorldPosition(mEntityID);
        isShaking = true;

    }

    private void NextScene()
    {

        Startfade();
        
    }
    public void OnMouseEnter()
    {
        zRotAngle = 229.3f;
        //InternalCalls.PlaySound(mEntityID, "SpinningDiscPS1_SFX");
        Vector3 MousePos = InternalCalls.GetMousePosWorld(0.0f);
        //tch: the accurate distance doesnt really matter i guess
        toCDDist = (MousePos - originalPosition).Length();
        isHovered = true;
    }

    public void OnMouseExit()
    {
        isHovered = false;
        string tag = InternalCalls.GetTag(mEntityID);
        if (tag == "NewGameCDChild")
        {
            InternalCalls.SetWorldRotation(mEntityID, ref originalRotation);
            InternalCalls.PauseSound(mEntityID, "SpinningDiscPS1_SFX");
            isSpinningSoundPlaying = false;
        }
        else if (tag == "ContinueCDChild")
        {
            InternalCalls.SetWorldRotation(mEntityID, ref originalRotation);
            InternalCalls.PauseSound(mEntityID, "SpinningDiscPS1_SFX");
            isSpinningSoundPlaying = false;
        }
    }
}


