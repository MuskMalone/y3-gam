using IGE.Utils;
using System;
using System.Runtime.CompilerServices;

public class SettingsButtons : Entity
{
  public bool IsClicked = false;



  void Start()
  {

  }

  void Update()
  {

  }

  public void OnPointerExit()
  {
    IsClicked = false;
  }


  public void OnPointerDown()
  {
    IsClicked = true;
    Debug.Log("HIT\n");
  }

  public void OnPointerUp()
  {
    if (IsClicked)
    {
      IsClicked = false;
    }
  }
}
