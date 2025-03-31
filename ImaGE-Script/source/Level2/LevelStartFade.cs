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

public class LevelStartFade : Entity
{
  public float fadeDuration = 3f;
  public Transition transition;



  void Start()
  {
    transition.StartTransition(true, fadeDuration, Transition.TransitionType.TV_SWITCH);
    if(InternalCalls.GetCurrentScene() == "..\\Assets\\Scenes\\Level2.scn")
        {

        }
  }

  void Update()
  {
    
  }
}
