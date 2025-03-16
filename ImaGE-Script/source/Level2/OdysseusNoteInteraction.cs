using System;
using IGE.Utils;

public class OdysseusNoteInteraction : Entity
{
    public PlayerMove playerMove;
    public Entity odysseusUI;
    public PlayerInteraction playerInteraction;
    public Entity viewNoteUI;  // UI prompt for "Press Click to View"
    public string noteTag = "OdysseusNote";
    public OdysseusNoteInteraction() : base() { }

    void Start()
    {
        if (odysseusUI == null)
        {
            Debug.LogError("[OdysseusNoteInteraction.cs] Odysseus UI Entity not found!");
            return;
        }

        odysseusUI.SetActive(false);

        if (playerMove == null)
        {
            Debug.LogError("[OdysseusNoteInteraction.cs] PlayerMove Script Entity not found!");
            return;
        }

        if (viewNoteUI == null)
        {
            Debug.LogError("[OdysseusNoteInteraction.cs] View Note UI Entity not found!");
            return;
        }

        viewNoteUI.SetActive(false); // Hide prompt at the start
    }

    //void Update()
    //{
    //    bool mouseClicked = Input.GetMouseButtonTriggered(0);
    //    bool isNoteHit = playerInteraction.RayHitString == noteTag;
    //    bool noteIsActive = odysseusUI.IsActive();

    //    // Show "Press Click to View" UI when hovering over note
    //    viewNoteUI.SetActive(isNoteHit && !noteIsActive);

    //    if (mouseClicked)
    //    {
    //        if (isNoteHit && !noteIsActive)
    //        {
    //            ShowNoteUI();
    //        }
    //        else if (noteIsActive)
    //        {
    //            HideNoteUI();
    //        }
    //    }

    //    viewNoteUI.SetActive(isNoteHit);
    //}

    void Update()
    {
        bool mouseClicked = Input.GetMouseButtonTriggered(0);
        bool isNoteHit = playerInteraction.RayHitString == noteTag;
        bool noteIsActive = odysseusUI.IsActive();

        // Ensure "Press Click to View" UI only shows when hovering over the note and it's not open
        if (!noteIsActive)
        {
            viewNoteUI.SetActive(isNoteHit);
        }
        else
        {
            viewNoteUI.SetActive(false); // Hide it when the note is opened
        }

        if (mouseClicked)
        {
            if (isNoteHit && !noteIsActive)
            {
                ShowNoteUI();
            }
            else if (noteIsActive)
            {
                HideNoteUI();
            }
        }
    }


    private void ShowNoteUI()
    {
        InternalCalls.PlaySound(mEntityID, "ViewNote");
        if (odysseusUI != null && playerMove != null)
        {
            odysseusUI.SetActive(true);
            playerMove.FreezePlayer();
            viewNoteUI.SetActive(false); // Hide hover text when note opens
        }
    }

    private void HideNoteUI()
    {
        InternalCalls.PlaySound(mEntityID, "ViewNote");
        if (odysseusUI != null && playerMove != null)
        {
            odysseusUI.SetActive(false);
            playerMove.UnfreezePlayer();
        }
    }
}
