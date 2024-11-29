using IGE.Utils;
using System;

public class SafeButtons : Entity
{
  public bool IsVisible;
  public bool IsClicked;

  void Start()
  {

  }

  void Update()
  {
    
  }

  public void OnPointerEnter()
  {
    string tag = InternalCalls.GetTag(mEntityID);
    Console.WriteLine("From MONO Enter" + tag);
    IsVisible = true;
  }

  public void OnPointerExit()
  {
    string tag = InternalCalls.GetTag(mEntityID);
    Console.WriteLine("From MONO Exit" + tag);
    IsVisible = false;
  }

  public void OnPointerDown()
  {
    string tag = InternalCalls.GetTag(mEntityID);
    Console.WriteLine("From MONO Down" + tag);
  }

  public void OnPointerUp()
  {
    string tag = InternalCalls.GetTag(mEntityID);
    Console.WriteLine("From MONO Up" + tag);
  }
}
