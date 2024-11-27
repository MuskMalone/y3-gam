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
    //    private Vector3 offset;         // Offset between mouse and CD position
    //    private bool isBeingDragged = false; // Whether the CD is being dragged
    //    private Vector3 initialPosition; // Initial position of the CD
    //    public Entity mainCamera;

    //    void Start()
    //    {
    //        // Save the initial position as System.Numerics.Vector3
    //        initialPosition = InternalCalls.GetPosition(mEntityID);

    //    }

    //    void Update()
    //    {
    //        if (isBeingDragged)
    //        {
    //            FollowMouseWithRay();
    //        }
    //    }

    //    public void OnMouseDown()
    //    {
    //        isBeingDragged = true;

    //        // Get the camera's position and direction using its entity ID
    //        uint mainCameraID = mainCamera.mEntityID; // Assuming mainCamera has a property `mEntityID`
    //        Vector3 cameraPosition = InternalCalls.GetMainCameraPosition(mainCameraID);
    //        Vector3 cameraDirection = InternalCalls.GetMainCameraDirection(mainCameraID);

    //        // Estimate the ray's end point
    //        Vector3 rayStart = cameraPosition;
    //        Vector3 rayEnd = rayStart + cameraDirection * 100.0f;

    //        // Perform the raycast from the CD entity
    //        uint hitEntity = InternalCalls.RaycastFromEntity(mEntityID, rayStart, rayEnd);

    //        if (hitEntity != 0)
    //        {
    //            // Get the CD's current position
    //            Vector3 cdPosition = InternalCalls.GetPosition(mEntityID);

    //            // Use the intersection point to calculate the offset
    //            Vector3 intersection = rayStart + cameraDirection * 10.0f; // Adjust as needed
    //            offset = cdPosition - intersection;
    //        }
    //    }

    //    public void OnMouseUp()
    //    {
    //        isBeingDragged = false;
    //    }

    //    private void FollowMouseWithRay()
    //    {
    //        // Get the camera's position and forward direction using its entity ID
    //        uint mainCameraID = mainCamera.mEntityID; // Assuming mainCamera has a property `mEntityID`
    //        Vector3 rayOrigin = InternalCalls.GetMainCameraPosition(mainCameraID);
    //        Vector3 rayDirection = InternalCalls.GetMainCameraDirection(mainCameraID);

    //        // Use these for raycasting or plane intersection
    //        Vector3 intersection = RayPlaneIntersection(rayOrigin, rayDirection, initialPosition.Y);

    //        // Calculate the new position for the CD
    //        Vector3 newPosition = intersection + offset;

    //        // Update the CD's position
    //        InternalCalls.SetPosition(mEntityID, ref newPosition);
    //    }

    //    private Vector3 RayPlaneIntersection(Vector3 rayOrigin, Vector3 rayDirection, float planeY)
    //    {
    //        // Assume the plane is parallel to the XZ plane at height planeY
    //        Vector3 planeNormal = new Vector3(0.0f, 1.0f, 0.0f); // Normal pointing up

    //        // Calculate the dot product
    //        float denominator = Vector3.Dot(rayDirection, planeNormal);

    //        if (Math.Abs(denominator) > 0.0001f) // Ensure the ray is not parallel to the plane
    //        {
    //            float t = (planeY - rayOrigin.Y) / rayDirection.Y;
    //            return rayOrigin + rayDirection * t;
    //        }

    //        // If the ray doesn't intersect the plane, return the ray's origin
    //        return rayOrigin;
    //    }
    //}

    //TEMPORARY
    public Entity CDinCase;
    public Entity CDCurr;

    public Entity fadeImageEntity; // Entity containing the fade Image
    private Image fadeImage; // Image used for the fade effect

    private Vec3<float> originalScale;
    private Vec3<float> hoverScale;
    public Vector3 outOfTheWay = new Vector3(10.0f, 10.0f, 10.0f);

    private bool isFading = false;
    private float fadeDuration = 3f;
    private float fadeElapsed = 0f;

    private float startAlpha = 0f; // Initial alpha
    private float targetAlpha = 1f; // Target alpha

    private bool isHovered = false;
    void Start()
    {
        CDinCase.SetActive(false);
        Vector3 scaleVector = InternalCalls.GetScale(mEntityID);
        originalScale = new Vec3<float>(scaleVector.X, scaleVector.Y, scaleVector.Z);
        hoverScale = new Vec3<float>(
            originalScale.X * 1.1f,
            originalScale.Y * 1.1f,
            originalScale.Z * 1.1f
        );

        if (fadeImageEntity != null)
        {
            fadeImage = fadeImageEntity.GetComponent<Image>();
            //fadeImage = FindImageInChildren(fadeImageEntity.mEntityID);
        }

        if (fadeImage != null)
        {
            Debug.Log("fadeimage is here");
            fadeImage.color = new Color(fadeImage.color.r, fadeImage.color.g, fadeImage.color.b, startAlpha);
        }

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
        if (isHovered)
        {
            Vector3 hoverVector = new Vector3(hoverScale.X, hoverScale.Y, hoverScale.Z);
            InternalCalls.SetScale(mEntityID, ref hoverVector);
        }
        else
        {
            Vector3 originalVector = new Vector3(originalScale.X, originalScale.Y, originalScale.Z);
            InternalCalls.SetScale(mEntityID, ref originalVector);
        }

        if (isFading)
        {
            fadeElapsed += Time.deltaTime;
            float alpha = Mathf.Lerp(startAlpha, targetAlpha, fadeElapsed / fadeDuration);

            if (fadeImage != null)
            {
                fadeImage.color = new Color(fadeImage.color.r, fadeImage.color.g, fadeImage.color.b, alpha);
            }
            if (fadeElapsed >= fadeDuration)
            {
                isFading = false;
                InternalCalls.SetCurrentScene("..\\Assets\\Scenes\\M3.scn");
            }
        }
    }



    private void Startfade()
    {
        isFading = true;
        fadeElapsed = 0f;
        //startAlpha = fadeImage != null ? fadeImage.color.a : 0f;
        //targetAlpha = 1f;
        //fadeDuration = 2.5f;
        //FadeManager.Instance.FadeToBlack(2.5f);
    }
    public void OnMouseDown()
    {
        CDinCase.SetActive(true);
        //CDCurr.SetActive(false);
        InternalCalls.SetWorldPosition(mEntityID, ref outOfTheWay);
        //Startfade();
        NextScene();
        
    }

    private void NextScene()
    {

        Startfade();
        
    }
    public void OnMouseEnter()
    {
        isHovered = true;
    }

    public void OnMouseExit()
    {
        isHovered = false;
    }
}


