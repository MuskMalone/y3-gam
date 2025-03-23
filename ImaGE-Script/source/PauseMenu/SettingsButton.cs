using IGE.Utils;
using System;

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
  }

  public void OnPointerUp()
  {
    if (IsClicked)
    {
      IsClicked = false;
    }
  }
}
