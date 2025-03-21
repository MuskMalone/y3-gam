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
