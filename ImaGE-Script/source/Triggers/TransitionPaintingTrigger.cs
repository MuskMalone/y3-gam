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

public class TransitionPaintingTrigger : Entity
{
  public Entity player;
  public Entity transitionPainting;

  // Start is called before the first frame update
  void Start()
  {
    transitionPainting.SetActive(false);
  }

  // Update is called once per frame
  void Update()
  {
    // unlock the rigidbody and let painting fall when player is close
    if (InternalCalls.OnTriggerEnter(mEntityID, player.mEntityID))
    {
      transitionPainting.SetActive(true);
      InternalCalls.SetGravityFactor(transitionPainting.mEntityID, 10f);
      InternalCalls.LockRigidBody(transitionPainting.mEntityID, false);
      Destroy();
    }
  }
}
