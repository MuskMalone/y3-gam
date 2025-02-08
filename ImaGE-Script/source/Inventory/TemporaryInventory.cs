/******************************************************************************/
//! \par        Image Engine
//! \file       TemporaryInventory.cs
//! \author     
//! \date       
//! \brief      Handles temporary inventory UI toggling.
//!
//! Copyright (C) 2024 DigiPen Institute of Technology. Reproduction
//! or disclosure of this file or its contents without the prior
//! written consent of DigiPen Institute of Technology is prohibited.
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

public class TemporaryInventory : Entity
{
    // Inventory Item UI (Image at the bottom right)
    public Entity artemisPaintingUI;
    public Entity artemisPaintingSelection;

    public Entity dionysusPaintingUI;
    public Entity dionysusPaintingSelection;

    private Entity activeUI = null; // Tracks the currently active UI

    // Start is called before the first frame update
    void Start()
    {
        // Ensure both UIs are hidden at the start
        if (artemisPaintingUI != null) artemisPaintingUI.SetActive(false);
        if (dionysusPaintingUI != null) dionysusPaintingUI.SetActive(false);
    }

    // Update is called once per frame
    void Update()
    {
        if (Input.anyKeyTriggered)
        {
            switch (Input.inputString)
            {
                case "1":
                    ToggleUI(artemisPaintingUI);
                    break;
                case "2":
                    ToggleUI(dionysusPaintingUI);
                    break;
            }
        }
    }


    private void ToggleUI(Entity ui)
    {
        if (ui == null) return;

        // If the clicked UI is already active, deactivate it
        if (ui == activeUI)
        {
            ui.SetActive(false);
            activeUI = null;
        }
        else
        {
            // Deactivate the currently active UI before activating the new one
            if (activeUI != null)
            {
                activeUI.SetActive(false);
            }

            // Activate the selected UI
            ui.SetActive(true);
            activeUI = ui;
        }
    }
}
