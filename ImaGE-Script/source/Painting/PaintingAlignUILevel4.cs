using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;
using System.Xml;
using IGE.Utils;

public class PaintingAlignUILevel4 : Entity
{
  private HoldupUI holdupUIScript;
  private Level4Inventory level4inventoryScript;
  private PictureAlign pictureAlignScript;

  public Entity bigPaintingUI;
  public Entity smallPaintingUI;
  public Entity alignmentUI;

  public bool isPainting = false;

  // Start is called before the first frame update
  void Start()
  {
    holdupUIScript = FindObjectOfType<HoldupUI>();
    level4inventoryScript = FindObjectOfType<Level4Inventory>();
    pictureAlignScript = FindObjectOfType<PictureAlign>();

    bigPaintingUI.SetActive(false);
    smallPaintingUI.SetActive(false);
    alignmentUI.SetActive(false);
  }

  // Update is called once per frame
  void Update()
  {
    // Ensure UI does not show if no valid item is selected
    if (level4inventoryScript.GetCurrentItem() == null || !level4inventoryScript.highlighted || !isPainting)
    {
      disableAlignUI();
      return;
    }

    if (holdupUIScript.isBigPaintingActive)
    {
      if (pictureAlignScript.IsAligned())
      {
        bigPaintingUI.SetActive(false);
        smallPaintingUI.SetActive(false);
        alignmentUI.SetActive(true);
      }
      else
      {
        bigPaintingUI.SetActive(true);
        smallPaintingUI.SetActive(false);
        alignmentUI.SetActive(false);
      }
    }
    else
    {
      bigPaintingUI.SetActive(false);
      smallPaintingUI.SetActive(true);
      alignmentUI.SetActive(false);
    }
  }


  public void disableAlignUI()
  {
    bigPaintingUI.SetActive(false);
    smallPaintingUI.SetActive(false);
    alignmentUI.SetActive(false);
  }

}