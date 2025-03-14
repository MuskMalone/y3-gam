using IGE.Utils;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Security.Permissions;
using System.Text;
using System.Threading.Tasks;
using System.Xml;

public class HexTableOrb : Entity
{
  public Entity glowingOrb, dimmedOrb;

  static readonly float riseVelocity = 4f;
  static readonly float intensity = 0.002f; // How much it vibrates
  static readonly float frequency = 0.05f;  // How fast it vibrates

  private enum State
  {
    INACTIVE,
    RISING,
    HOVERING
  } 

  private State currState = State.INACTIVE;
  private Vector3 originalPos;
  private float timeElapsed = 0f;

  // Start is called before the first frame update
  void Start()
  {
    glowingOrb.SetActive(false);
    dimmedOrb.SetActive(true);
  }

  // Update is called once per frame
  void Update()
  {
    switch (currState)
    {
      case State.INACTIVE:
        return;

      case State.RISING:
        {
          // when orb comes to a stop
          if (InternalCalls.GetVelocity(mEntityID).Y < 0.1f)
          {
            currState = State.HOVERING;
            originalPos = InternalCalls.GetPosition(mEntityID);
            StartGlowing();
          }

          break;
        }
      case State.HOVERING:
        {
          timeElapsed += Time.deltaTime;

          if (timeElapsed >= frequency)
          {
            // Generate a small random offset
            float x = Mathf.RandRange(-intensity, intensity);
            float y = Mathf.RandRange(-intensity, intensity);
            float z = Mathf.RandRange(-intensity, intensity);

            // Apply the offset but always return to the original position
            Vector3 newPos = originalPos + new Vector3(x, 0f, z);

            InternalCalls.SetPosition(mEntityID, ref newPos);
            InternalCalls.UpdatePhysicsToTransform(mEntityID);
            InternalCalls.SetAngularVelocity(mEntityID, new Vector3(999f, 999f, 999f));
            timeElapsed = 0f;
          }

          break;
        }
    }
  }

  private void StartGlowing()
  {
    dimmedOrb.SetActive(false);
    glowingOrb.SetActive(true);
  }

  public void Rise()
  {
    float denom = 1 / Time.deltaTime;
    InternalCalls.SetVelocity(mEntityID, new Vector3(0f, riseVelocity * denom, 0f));
    InternalCalls.SetAngularVelocity(mEntityID, 
      new Vector3(Mathf.RandRange(360f, 720f) * denom, Mathf.RandRange(360f, 720f) * denom, Mathf.RandRange(360f, 720f) * denom));
    currState = State.RISING;
  }
}
