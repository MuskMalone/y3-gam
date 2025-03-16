using IGE.Utils;
using System;
using System.Numerics;
using System.Text;

public class MomAngryTrigger : Entity
{
  public PlayerMove playerMove;
  public MomAngryTrigger() : base()
  {

  }

  void Start()
  {

  }

  void Update()
  {
    if (InternalCalls.OnTriggerEnter(mEntityID, playerMove.mEntityID))
    {
      InternalCalls.PlaySound(mEntityID, "AngryMom");
      SetActive(false);
    }
  }
}