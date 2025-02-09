using IGE.Utils;

public class TransitionToLevel3 : Entity
{
    public PlayerInteraction playerInteraction; // Handles raycasting
    public string targetObjectName = "Level2Frag"; 
    public string nextScenePath = "..\\Assets\\Scenes\\Level3.scn"; // Path to next scene

    public TransitionToLevel3() : base() { }

    void Start()
    {
        if (playerInteraction == null)
        {
            Debug.LogError("[Transitiontolevel3.cs] PlayerInteraction Script not found!");
            return;
        }
    }

    void Update()
    {
        bool mouseClicked = Input.GetMouseButtonTriggered(0);
        bool isObjectHit = playerInteraction.RayHitString == targetObjectName;

        if (mouseClicked && isObjectHit)
        {
            LoadNextScene();
        }
    }

    private void LoadNextScene()
    {
        //InternalCalls.PlaySound(mEntityID, "ConfirmClick"); // Optional click sound
        InternalCalls.SetCurrentScene(nextScenePath); // Load next scene
    }
}
