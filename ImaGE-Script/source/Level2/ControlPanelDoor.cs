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

public class ControlPanelDoor : Entity
{
    public PlayerInteraction playerInteraction;
    public ControlPanel2 controlPanel;
    public string doorOpenAnimation;
    public Entity doorEntity;

    private enum State
    {
        CLOSED,   // starting state
        OPENING,  // animation in progress
        OPENED,   // control panel fully opened
    }
    private State currState = State.CLOSED;

    public ControlPanelDoor() : base()
    {

    }

    // Start is called before the first frame update
    void Start()
    {

    }

    // Update is called once per frame
    void Update()
    {
        switch (currState)
        {
            // check for player interaction
            case State.CLOSED:
                bool isDoorHit = playerInteraction.RayHitString == InternalCalls.GetTag(doorEntity.mEntityID);

                // if interacted with, trigger the animation
                if (isDoorHit && Input.GetMouseButtonTriggered(0))
                {
                    InternalCalls.PlayAnimation(mEntityID, doorOpenAnimation);
                    currState = State.OPENING;
                    controlPanel.Unlock();  // we'll unlock the interaction the moment the animation starts
                }

                break;

            // animation: align the collider with the transform
            case State.OPENING:

                // update collider while animation is playing
                InternalCalls.UpdatePhysicsToTransform(doorEntity.mEntityID);

                // switch state when animation ends
                if (!InternalCalls.IsPlayingAnimation(mEntityID))
                {
                    currState = State.OPENED;
                }
                break;

            // animation completed: destroy this script
            case State.OPENED:
                Destroy(this);

                break;
            default:
                break;
        }
    }
}


