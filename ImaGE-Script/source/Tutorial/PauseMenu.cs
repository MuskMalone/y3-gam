using IGE.Utils;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;
using System.Xml;

public class PauseMenu : Entity
{
    public Entity imageEntity; // The entity containing the pause menu UI
    private bool isPaused = false; // Tracks pause state
    private PlayerMove playerMove; // Reference to player movement script

    void Start()
    {
        if (imageEntity != null)
        {
            imageEntity.SetActive(false); // Ensure menu is off at the start
        }

        playerMove = FindObjectOfType<PlayerMove>(); // Get the player movement script
        if (playerMove == null) Debug.LogError("PlayerMove component not found!");
    }

    void Update()
    {
        if (Input.GetKeyDown(KeyCode.P)) // Press 'P' to toggle pause
        {
            isPaused = !isPaused;

            if (imageEntity != null)
            {
                imageEntity.SetActive(isPaused);
            }

            if (playerMove != null)
            {
                if (isPaused)
                {
                    playerMove.FreezePlayer(); // Freeze movement
                }
                else
                {
                    playerMove.UnfreezePlayer(); // Resume movement
                }
            }
        }
    }
}
