using IGE.Utils;

public class NoteInteraction : Entity
{

  public PlayerMove playerMove;
  public Entity noteUI;
  public PlayerInteraction playerInteraction;
  public Entity viewNoteUI;

  public NoteInteraction() : base()
  {

  }

  void Start()
  {
    if (noteUI == null)
    {
      Debug.LogError("[NoteInteraction.cs] Note UI Entity not found!");
      return;
    }

    noteUI?.SetActive(false);
    viewNoteUI?.SetActive(false);

    if (playerMove == null)
    {
      Debug.LogError("[NoteInteraction.cs] PlayerMove Script Entity not found!");
      return;
    }
  }

  void Update()
  {
    bool mouseClicked = Input.GetMouseButtonTriggered(0);
    bool isNoteHit = playerInteraction.RayHitString == "Mom's Note";
    bool noteIsActive = noteUI.IsActive();

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

    viewNoteUI.SetActive(isNoteHit);
  }

  private void ShowNoteUI()
  {
    InternalCalls.PlaySound(mEntityID, "ViewNote");
    if (noteUI != null && playerMove != null)
    {
      noteUI.SetActive(true);
      playerMove.FreezePlayer();
      viewNoteUI.SetActive(false);
    }
  }

  private void HideNoteUI()
  {
    InternalCalls.PlaySound(mEntityID, "ViewNote");
    if (noteUI != null && playerMove != null)
    {
      noteUI.SetActive(false);
      playerMove.UnfreezePlayer();
    }
  }
}