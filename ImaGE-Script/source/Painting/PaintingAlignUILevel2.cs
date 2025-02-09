/******************************************************************************/
/*!
\par        Image Engine
\file       .cs

\author     
\date       

\brief      


Copyright (C) 2024 DigiPen Institute of Technology. Reproduction
or disclosure of this file or its contents without the prior
written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/


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

public class PaintingAlignUILevel2 : Entity
{
    private HoldupUI holdupUIScript;
    private Level2Inventory level2inventoryScript;
    private PictureAlign pictureAlignScript;

    public Entity bigPaintingUI;
    public Entity smallPaintingUI;
    public Entity alignmentUI;

    public bool isPainting = false;

    // Start is called before the first frame update
    void Start()
    {
        holdupUIScript = FindObjectOfType<HoldupUI>();
        level2inventoryScript = FindObjectOfType<Level2Inventory>();
        pictureAlignScript = FindObjectOfType<PictureAlign>();

        bigPaintingUI.SetActive(false);
        smallPaintingUI.SetActive(false);
        alignmentUI.SetActive(false);
    }

    // Update is called once per frame
    void Update()
    {
        if (level2inventoryScript.highlighted && isPainting)
        {
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
        else
        {
            bigPaintingUI.SetActive(false);
            smallPaintingUI.SetActive(false);
            alignmentUI.SetActive(false);
        }

    }

}



