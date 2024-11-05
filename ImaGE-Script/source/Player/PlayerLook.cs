using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Numerics;
using Image.Utils;
public class PlayerLook : Entity
{
    public Camera cam;
    private float xRotation = 0f;

    public float xSensitivity = 30.0f;
    public float ySensitivity = 30.0f;

    // New flag to control whether the player can look around
    public bool canLook = true;

    private void Start()
    {
        Cursor.lockState = CursorLockMode.Locked;
        Cursor.visible = false;
    }

    public void ProcessLook(Vector2 input)
    {
        if (!canLook) return;  // Skip look processing if the player is frozen

        float mouseX = input.X;
        float mouseY = input.Y;

        // Calculate camera rotation for looking up and down
        xRotation -= (mouseY * Time.deltaTime) * ySensitivity;
        xRotation = Mathf.Clamp(xRotation, -90.0f, 90.0f);

        // Apply this to our camera transform.
        cam.transform.localRotation = Mathf.EulertoQuat(new Vector3(xRotation, 0, 0));

        // Rotate player to look left and right
        GetComponent<Transform>().Rotate(new Vector3(0,1,0) * (mouseX * Time.deltaTime) * xSensitivity);
    }
}

