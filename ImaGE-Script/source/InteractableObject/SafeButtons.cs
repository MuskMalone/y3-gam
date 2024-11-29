using IGE.Utils;
using System;

public class SafeButtons : Entity
{
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
  }

  public void OnPointerExit()
  {
    string tag = InternalCalls.GetTag(mEntityID);
    Console.WriteLine("From MONO Exit" + tag);
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
