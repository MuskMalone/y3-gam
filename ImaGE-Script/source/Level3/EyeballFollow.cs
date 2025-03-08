/******************************************************************************/
/*!
\par        Image Engine
\file       EyeBallFollow.cs

\author     
\date       

\brief      
this script will ensure the eyeball will constantly rotate to follow the player

Copyright (C) 2024 DigiPen Institute of Technology. Reproduction
or disclosure of this file or its contents without the prior
written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/


using IGE.Utils;
using System;
using System.Numerics;
using System.Text;

public class EyeBallFollow: Entity
{
  public Entity player;
  public float RotationSpeed { get; set; } = 5f;  // Speed at which the eyeball rotates (in radians per second)
  private static readonly Vector3 LocalForward = -Vector3.UnitZ;
  EyeBallFollow() : base()
  {
      
  }

  // Start is called before the first frame update
  void Start()
  {

  }

  // Update is called once per frame
  void Update()
  {



    // Calculate the direction from the eyeball to the player
    Vector3 directionToPlayer = Vector3.Normalize(player.GetComponent<Transform>().worldPosition - GetComponent<Transform>().worldPosition);

    // Transform the local forward vector to world space using the eyeball's current rotation
    Vector3 worldForward = Vector3.Transform(LocalForward, GetComponent<Transform>().rotation);

    // Calculate the rotation needed to align the world forward vector with the direction to the player
    Quaternion targetRotation = Mathf.QuaternionFromToRot(worldForward, directionToPlayer);

    // Apply the target rotation to the eyeball's current rotation
    GetComponent<Transform>().rotation = Quaternion.Slerp(GetComponent<Transform>().rotation, targetRotation * GetComponent<Transform>().rotation, RotationSpeed * Time.deltaTime);
  }
}


