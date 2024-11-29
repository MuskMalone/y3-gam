using IGE.Utils;
using System;

public class SafeButtons : Entity
{
  public bool IsVisible = false;
  public bool IsClicked = false;

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

  }

  public void OnPointerUp()
  {

  }
}
