using IGE.Utils;
using System.Numerics;

// Script to be placed in parent of black borders folder (its in the prefab too)
public class BlackBorder : Entity
{
  public Entity blackBorder1;
  public Entity blackBorder2;
  public Entity centerDot;
  
  void Start()
  {
    blackBorder1?.SetActive(false);
    blackBorder2?.SetActive(false);
  }

  void Update()
  {
    
  }

  // To be called by other scripts
  public void DisplayBlackBorders()
  {
    blackBorder1?.SetActive(true);
    blackBorder2?.SetActive(true);
    centerDot?.SetActive(false);
  }

  public void HideBlackBorders()
  {
    blackBorder1?.SetActive(false);
    blackBorder2?.SetActive(false);
    centerDot?.SetActive(true);
  }
}

