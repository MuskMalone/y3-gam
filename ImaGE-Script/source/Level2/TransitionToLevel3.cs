using IGE.Utils;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Security.Cryptography;
using System.Text;
using System.Threading.Tasks;
using System.Xml;

public class TransitionToLevel3 : Entity
{
  public Entity player;
  public string nextScenePath = "..\\Assets\\Scenes\\Level3.scn"; // Path to next scene
  public Transition transition;

  private bool playerEntered = false;

  // Start is called before the first frame update
  void Start()
  {

  }

  // Update is called once per frame
  void Update()
  {
    if (!playerEntered)
    {
      if (InternalCalls.OnTriggerEnter(mEntityID, player.mEntityID))
      {
        transition.StartTransition(true, 1.5f, Transition.TransitionType.FADE);
        playerEntered = true;
      }

      return;
    }

    if (transition.IsFinished())
    {
      InternalCalls.SetCurrentScene(nextScenePath); // Load next scene
    }
  }
}
