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

public class CDDragAndDrop : Entity
{
    //private Vec3<float> offset;         // Offset between mouse and CD position
    //private bool isBeingDragged = false; // Whether the CD is being dragged
    //private Vec3<float> initialPosition; // Initial position of the CD

    //void Start()
    //{
    //    initialPosition = InternalCalls.GetPosition(mEntityID); // Save the initial position
    //}

    //void Update()
    //{
    //    if (isBeingDragged)
    //    {
    //        FollowMouseWithRay();
    //    }
    //}

    //public void OnMouseDown()
    //{
    //    isBeingDragged = true;

    //    // Calculate the offset between the mouse ray and the CD's position
    //    Vec3<float> rayOrigin = InternalCalls.GetMainCameraPosition(FindEntityByTag("MainCamera").mEntityID);
    //    Vec3<float> rayDirection = InternalCalls.GetMainCameraDirection(FindEntityByTag("MainCamera").mEntityID);

    //    Vec3<float> cdPosition = InternalCalls.GetPosition(mEntityID);
    //    Vec3<float> intersection = RayPlaneIntersection(rayOrigin, rayDirection, cdPosition.Y);

    //    offset = new Vec3<float>(
    //        cdPosition.X - intersection.X,
    //        cdPosition.Y - intersection.Y,
    //        cdPosition.Z - intersection.Z
    //    );
    //}

    //public void OnMouseUp()
    //{
    //    isBeingDragged = false;
    //}

    //private void FollowMouseWithRay()
    //{
    //    // Get the camera's ray
    //    Vec3<float> rayOrigin = InternalCalls.GetMainCameraPosition(FindEntityByTag("MainCamera").mEntityID);
    //    Vec3<float> rayDirection = InternalCalls.GetMainCameraDirection(FindEntityByTag("MainCamera").mEntityID);

    //    // Find where the ray intersects with the plane
    //    Vec3<float> intersection = RayPlaneIntersection(rayOrigin, rayDirection, initialPosition.Y);

    //    // Apply the offset and update the CD's position
    //    Vec3<float> newPosition = new Vec3<float>(
    //        intersection.X + offset.X,
    //        intersection.Y + offset.Y,
    //        intersection.Z + offset.Z
    //    );
    //    InternalCalls.SetPosition(mEntityID, ref newPosition);
    //}

    //private Vec3<float> RayPlaneIntersection(Vec3<float> rayOrigin, Vec3<float> rayDirection, float planeY)
    //{
    //    // Assume the plane is parallel to the XZ plane at height planeY
    //    Vec3<float> planeNormal = new Vec3<float>(0.0f, 1.0f, 0.0f); // Normal pointing up

    //    // Calculate the dot product manually
    //    float denominator = rayDirection.X * planeNormal.X +
    //                        rayDirection.Y * planeNormal.Y +
    //                        rayDirection.Z * planeNormal.Z;

    //    if (Math.Abs(denominator) > 0.0001f) // Ensure the ray is not parallel to the plane
    //    {
    //        float t = (planeY - rayOrigin.Y) / rayDirection.Y;
    //        return new Vec3<float>(
    //            rayOrigin.X + rayDirection.X * t,
    //            rayOrigin.Y + rayDirection.Y * t,
    //            rayOrigin.Z + rayDirection.Z * t
    //        );
    //    }

    //    // Return the ray's origin if it doesn't intersect (fallback)
    //    return rayOrigin;
    //}

    //TEMPORARY
    public Entity CDinCase;
    public Entity CDCurr;
    void Start()
    {
        CDinCase.SetActive(false);
    }

    void Update()
    {

    }

    public void OnMouseDown()
    {
        CDinCase.SetActive(true);
        CDCurr.SetActive(false);
    }
}


