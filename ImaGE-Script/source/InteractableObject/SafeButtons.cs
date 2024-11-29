using IGE.Utils;
using System;

public class SafeButtons : Entity
{
  public bool IsVisible = false;
  public bool IsClicked = false;
  public bool TriggerButton = false;

  void Start()
  {

  }

  void Update()
  {
    
  }

  public void OnPointerEnter()
  {
    IsVisible = true;
  }

  public void OnPointerExit()
  {
    IsVisible = false;
  }

  public void OnPointerDown()
  {
    IsClicked = true;
  }

  public void OnPointerUp()
  {
    if (IsClicked)
    {
      TriggerButton = true;
      IsClicked = false;
    }
  }
}
