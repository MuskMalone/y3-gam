using IGE.Utils;

public class NoteInteraction : Entity
{

  public PlayerMove playerMove;
  public Entity noteUI;
  public PlayerInteraction playerInteraction;

  public NoteInteraction() : base()
  {

  }

  void Start()
  {
    if (noteUI == null)
    {
      Debug.LogError("[Dialogue.cs] Note UI Entity not found!");
      return;
    }

    noteUI?.SetActive(false);

    if (playerMove == null)
    {
      Debug.LogError("[Dialogue.cs] PlayerMove Script Entity not found!");
      return;
    }
  }

  void Update()
  {
    if (Input.GetMouseButtonTriggered(0) && playerInteraction.RayHitString == "Mom's Note" && !noteUI.IsActive())
    {
      ShowNoteUI();
    }

    else if (noteUI.IsActive() && Input.GetMouseButtonTriggered(0))
    {
      HideNoteUI();
    }
  }

  private void ShowNoteUI()
  {
    if (noteUI != null && playerMove != null)
    {
      noteUI.SetActive(true);
      playerMove.FreezePlayer();
    }
  }

  private void HideNoteUI()
  {
    if (noteUI != null && playerMove != null)
    {
      noteUI.SetActive(false);
      playerMove.UnfreezePlayer();
    }
  }
}