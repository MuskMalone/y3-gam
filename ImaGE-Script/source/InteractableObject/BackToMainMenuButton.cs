using IGE.Utils;
using System.Numerics;

public class BackToMainMenuButton : Entity
{
  void Start()
  {
    InternalCalls.ShowCursor();
  }

  void Update()
  {
    
  }

  public void OnPointerDown()
  {
    InternalCalls.SetCurrentScene("..\\Assets\\Scenes\\mainmenu.scn");
  }
}
