using IGE.Utils;
using System;
using System.Runtime.CompilerServices;

public class SettingsButtons : Entity
{
  public bool IsClicked = false;
  public Entity Text;
  public Entity Border;


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
    //Text.SetActive(true);
    //Border.SetActive(true);
  }

  public void OnPointerUp()
  {
    if (IsClicked)
    {
      IsClicked = false;
    }
  }
}
