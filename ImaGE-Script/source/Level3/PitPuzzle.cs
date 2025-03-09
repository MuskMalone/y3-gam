/******************************************************************************/
/*! 
\par        Image Engine 
\file       PitPuzzle.cs

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
using IGE.Utils;

public class PitPuzzle : Entity
{
    public Entity VisiblePlanks;
    public Entity InvisiblePlanks;

    // Tracks whether the planks are switched (visible planks active)
    private bool arePlanksSwitched = false;

    void Start()
    {
        // Initial state: visible planks off, invisible planks on
        VisiblePlanks.SetActive(false);
        InvisiblePlanks.SetActive(true);
    }

    void Update()
    {
        // For testing purposes
        if (Input.GetKeyTriggered(KeyCode.EQUAL))
        {
            // Toggle the state
            arePlanksSwitched = !arePlanksSwitched;
            ApplyPlankState();
        }
    }

    // Helper function to apply the current state
    private void ApplyPlankState()
    {
        if (arePlanksSwitched)
        {
            VisiblePlanks.SetActive(true);
            InvisiblePlanks.SetActive(false);
        }
        else
        {
            VisiblePlanks.SetActive(false);
            InvisiblePlanks.SetActive(true);
        }
    }

    // If you want to switch via a method call as well
    public void switchPlanks()
    {
        VisiblePlanks.SetActive(true);
        InvisiblePlanks.SetActive(false);
    }
}
