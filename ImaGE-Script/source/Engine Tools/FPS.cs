using IGE.Utils;
using System.Numerics;

public class FPS : Entity
{

  void Start()
  {

  }

  void Update()
  {
    InternalCalls.SetText(mEntityID, "FPS: " + InternalCalls.GetFPS().ToString());
  }
}