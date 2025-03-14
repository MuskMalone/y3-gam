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
  public Entity brokenOrb;
  public Entity tableSensor;

  static readonly float violentDuration = 5f;
  static readonly float riseVelocity = 4f;
  static readonly float intensity = 0.002f; // How much it vibrates
  static readonly float frequency = 0.05f;  // How fast it vibrates
  static readonly float violentIntensity = intensity * 4f; // How much it vibrates
  static readonly float violentFrequency = frequency * 0.5f;  // How fast it vibrates

  private enum State
  {
    INACTIVE,
    RISING,
    HOVERING,
    VIOLENT,
    FALLING
  } 

  private State currState = State.INACTIVE;
  private Vector3 originalPos;
  private float timeElapsed = 0f, violentTimer = 0f;

  // Start is called before the first frame update
  void Start()
  {
    StopGlowing();
    brokenOrb.SetActive(false);
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

      case State.VIOLENT:
        {
          timeElapsed += Time.deltaTime;
          violentTimer += Time.deltaTime;

          if (timeElapsed >= violentDuration)
          {
            Fall();
            timeElapsed = 0f;
            return;
          }
          else if (violentTimer >= violentFrequency)
          {
            // Generate a small random offset
            float x = Mathf.RandRange(-violentIntensity, violentIntensity);
            float y = Mathf.RandRange(-violentIntensity, violentIntensity);
            float z = Mathf.RandRange(-violentIntensity, violentIntensity);

            // Apply the offset but always return to the original position
            Vector3 newPos = originalPos + new Vector3(x, 0f, z);

            InternalCalls.SetPosition(mEntityID, ref newPos);
            InternalCalls.SetAngularVelocity(mEntityID, new Vector3(999f, 999f, 999f));
            violentTimer = 0f;
          }

          break;
        }

      // in this state, the orb slowly rolls off the table
      case State.FALLING:
        {
          // align all colliders to their pieces
          //foreach (uint id in InternalCalls.GetAllChildren(brokenOrb.mEntityID))
          //{
          //  InternalCalls.UpdatePhysicsToTransform(id);
          //}

          // if orb hasn't left the table, do nothing
          if (!InternalCalls.OnTriggerExit(tableSensor.mEntityID, mEntityID)) { return; }

          // orb is falliing off now, swap to brokenOrb
          SetActive(false);
          brokenOrb.SetActive(true);  // activate the broken orb and all its pieces
          InternalCalls.UnparentEntity(brokenOrb.mEntityID);

          Destroy(this);  // we are done here

          break;
        }
    }
  }

  public void Rise()
  {
    float denom = 1 / Time.deltaTime;
    InternalCalls.SetVelocity(mEntityID, new Vector3(0f, riseVelocity * denom, 0f));
    InternalCalls.SetAngularVelocity(mEntityID, 
      new Vector3(Mathf.RandRange(360f, 720f) * denom, Mathf.RandRange(360f, 720f) * denom, Mathf.RandRange(360f, 720f) * denom));
    currState = State.RISING;
  }

  public void LosePower()
  {
    currState = State.VIOLENT;
  }

  private void Fall()
  {
    StopGlowing();

    // remove all velocity
    InternalCalls.SetVelocity(mEntityID, Vector3.Zero);
    InternalCalls.SetAngularVelocity(mEntityID, Vector3.Zero);
    InternalCalls.SetGravityFactor(mEntityID, 10f);

    currState = State.FALLING;
  }

  private void StartGlowing()
  {
    dimmedOrb.SetActive(false);
    glowingOrb.SetActive(true);
  }

  private void StopGlowing()
  {
    dimmedOrb.SetActive(true);
    glowingOrb.SetActive(false);
  }
}