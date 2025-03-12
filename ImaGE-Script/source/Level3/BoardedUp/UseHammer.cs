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

public class UseHammer : Entity
{
  public Entity Nail1, Nail2;
  public Entity Nail3;
  public Entity Nail4, Nail5;
  public String[] animations; // 1 animation for each plank

  private Entity[] planksAndNails;  // <plank, nail, nail, plank, ...>
  private int currIndex = -1;

  // Start is called before the first frame update
  void Start()
  {
    planksAndNails = new Entity[] {
      Nail1 , Nail2,
      Nail3, null,
      Nail4, Nail5
    };
  }

  // Update is called once per frame
  void Update()
  {
    if (InternalCalls.IsKeyTriggered(KeyCode.H))
    {
      SetActive(true);
    }

    if (!IsActive()) { return; }

    // animation active: align collider to transform
    if (InternalCalls.IsPlayingAnimation(mEntityID))
    {
      //InternalCalls.UpdatePhysicsToTransform(mEntityID);
    }
    // we reached the end of the animation, trigger the next one
    else
    {
      // if no more animations, destroy this script
      if (++currIndex >= animations.Length)
      {
        SetActive(false);
        //Destroy(this);
        return;
      }

      // make board drop

      // trigger hammer anim
      InternalCalls.PlayAnimation(mEntityID, animations[currIndex]);

      // also trigger the anim for both nails
      int offset = currIndex * 2;
      planksAndNails[offset]?.FindScript<Nail>().TriggerAnim();
      if (offset == 0)
      {
        planksAndNails[offset + 1]?.FindScript<TwoPlankNail>().TriggerAnim();
      }
      else
      {
        planksAndNails[offset + 1]?.FindScript<Nail>().TriggerAnim();
      }
    }
  }
}


