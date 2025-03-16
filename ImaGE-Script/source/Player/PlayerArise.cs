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
using System.Numerics;
using System.Text;


public class PlayerArise  : Entity
{
  public bool hasArsied = false;
  static private readonly string PLayerAriseAnim = "PlayerArise.anim";
  public PlayerMove playerMove;
  // Start is called before the first frame update

  PlayerArise(): base()
  {

  }
  void Start()
  {

  }

  // Update is called once per frame
  void Update()
  {
    if (!hasArsied)
    {
      if (playerMove != null)
      {
        playerMove.useScriptRotation = false;
      }
      InternalCalls.PlayAnimation(mEntityID, PLayerAriseAnim);
      InternalCalls.UpdatePhysicsToTransform(mEntityID);
      hasArsied = true;
    }

    else if (!InternalCalls.IsPlayingAnimation(mEntityID))
    {
      playerMove.SetRotation(GetComponent<Transform>().rotationEuler);
      playerMove.useScriptRotation = true;
      Destroy(this);
    }
  }
}


