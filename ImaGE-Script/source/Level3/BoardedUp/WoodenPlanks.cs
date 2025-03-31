using IGE.Utils;
using System.Numerics;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;
using System.Xml;

public class WoodenPlanks : Entity
{
  public Level3Inventory inventoryScript;
  public PlayerInteraction playerInteraction;
  public HammerLevel3 Hammer;
  public Level3Dialogue dialogue;
  public Entity interactUI;
  public Entity Door;
  public string doorAnimName;
  public string[] notStrongEnoughDialogue;

    private bool isDialogueActive = false;

    private enum State
  {
    BLOCKED,
    USING_HAMMER,
    OPENING_DOOR,
    BYE_BYE
  }
  private State currState = State.BLOCKED;

  // Start is called before the first frame update
  void Start()
  {

  }

  // Update is called once per frame
  void Update()
  {
    switch (currState)
    {
      case State.BLOCKED:
        {
          bool isPlanksHit = playerInteraction.RayHitString == InternalCalls.GetTag(mEntityID);
          if (Input.GetMouseButtonTriggered(0) && isPlanksHit)
          {
            if (!inventoryScript.hammerEquipped && !dialogue.isInDialogueMode)
            {
              // Play locked sound
              dialogue.SetDialogue(notStrongEnoughDialogue, new Level3Dialogue.Emotion[] {
                Level3Dialogue.Emotion.Thinking
              });
              isDialogueActive = true;
              return;
            }

            if (inventoryScript.hammerEquipped)
            {
              inventoryScript.RemoveItem(Hammer);
              currState = State.USING_HAMMER;
              interactUI.SetActive(false);
            }
          }
          else
          {
            interactUI?.SetActive(isPlanksHit);
          }

          break;
        }

        case State.USING_HAMMER:
        {
          if (Hammer.GetState() != HammerLevel3.HammerState.COMPLETE)
          {
            // may want to do some camera stuff here

            return; 
          }

          SetActive(false);
          InternalCalls.PlayAnimation(Door.mEntityID, doorAnimName);
          currState = State.OPENING_DOOR;

          // workaround to disabling a collider - just teleporting it out of the map
          // its okay! this entity will be destroyed later anyway
          Vector3 outOfWorld = new Vector3(0f, -300f, 0f);
          InternalCalls.SetWorldPosition(mEntityID, ref outOfWorld);
          InternalCalls.UpdatePhysicsToTransform(mEntityID);

          break;
        }

      case State.OPENING_DOOR:
        {
          // align the door's collider to its animation
          if (InternalCalls.IsPlayingAnimation(Door.mEntityID)) 
          {
            InternalCalls.UpdatePhysicsToTransform(Door.mEntityID);
            return;
          }
          
          // animation complete, enter last phase
          currState = State.BYE_BYE;

          break;
        }

      // destroy both hammer and this script
      case State.BYE_BYE:
        {
          InternalCalls.DestroyEntity(Hammer.mEntityID);
          Destroy();

          break;
        }
    }


        if(isDialogueActive)
        {
            if(dialogue.CurrentLineIndex == 0)
            {
                InternalCalls.PlaySound(mEntityID, "L3_2");
                isDialogueActive = false;
            }

        }
  }
}
