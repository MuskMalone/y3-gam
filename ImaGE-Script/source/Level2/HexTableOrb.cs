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
  public Entity tableSurface;
  public Entity tableSensor;

  static readonly float violentDuration = 5f;
  static readonly float riseVelocity = 4f;
  static readonly float intensity = 0.002f; // How much it vibrates
  static readonly float frequency = 0.05f;  // How fast it vibrates
  static readonly float violentIntensity = intensity * 3f; // How much it vibrates
  static readonly float violentFrequency = frequency * 0.3f;  // How fast it vibrates
  static readonly Vector3 tableSurfaceMidpoint = new Vector3(63.429f, 67.564f, -425.342f);
  static readonly float pushOffStrength = 45f;

  private enum State
  {
    INACTIVE,
    RISING,
    HOVERING,
    VIOLENT,
    FALLING,
    ROLLING,
    OFF_TABLE,
  } 

  private State currState = State.INACTIVE;
  private Vector3 originalPos;
  private float timeElapsed = 0f, violentTimer = 0f;
  private bool firstFrameAfterFall = true;

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
            float z = Mathf.RandRange(-intensity, intensity);

            // Apply the offset but always return to the original position
            Vector3 newPos = originalPos + new Vector3(x, 0f, z);

            InternalCalls.SetPosition(mEntityID, ref newPos);
            InternalCalls.UpdatePhysicsToTransform(mEntityID);
            InternalCalls.SetAngularVelocity(mEntityID, new Vector3(999f, 999f, 999f));
            timeElapsed -= frequency;
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
            float currIntensity = Mathf.Lerp(intensity, violentIntensity, timeElapsed / violentDuration);

            // Generate a small random offset
            float x = Mathf.RandRange(-currIntensity, currIntensity);
            float y = Mathf.RandRange(-currIntensity, currIntensity);
            float z = Mathf.RandRange(-currIntensity, currIntensity);

            // Apply the offset but always return to the original position
            Vector3 newPos = originalPos + new Vector3(x, y, z);

            InternalCalls.SetPosition(mEntityID, ref newPos);
            InternalCalls.UpdatePhysicsToTransform(mEntityID);
            InternalCalls.SetAngularVelocity(mEntityID, new Vector3(999f, 999f, 999f));
            violentTimer -= violentFrequency;
          }

          break;
        }

      // in this state, the orb slowly rolls off the table
      case State.FALLING:
        {
          // wait until orb lands on table
          if (InternalCalls.GetContactPoints(tableSurface.mEntityID, mEntityID).Length > 0)
          {
            currState = State.ROLLING;
          }

          break;
        }

      case State.ROLLING:
        {
          // if orb has rolled off
          if (InternalCalls.GetVelocity(mEntityID).Y < -2f)
          {
            // orb is falliing off now, swap to brokenOrb
            SetActive(false);
            brokenOrb.SetActive(true);  // activate the broken orb and all its pieces
            InternalCalls.UnparentEntity(brokenOrb.mEntityID);
            currState = State.OFF_TABLE;

            // align all colliders to their pieces
            foreach (uint id in InternalCalls.GetAllChildren(brokenOrb.mEntityID))
            {
              InternalCalls.UpdatePhysicsToTransform(id);
            }
            return;
          }

          Vector3 forceDir = InternalCalls.GetWorldPosition(mEntityID) - tableSurfaceMidpoint;
          forceDir = Vector3.Normalize(new Vector3(forceDir.X, 0f, forceDir.Z));
          InternalCalls.SetVelocity(mEntityID, forceDir * pushOffStrength);

          Vector3 angularDirection = Vector3.Normalize(Vector3.Cross(Vector3.UnitY, forceDir));
          InternalCalls.SetAngularVelocity(mEntityID, angularDirection * pushOffStrength);


          break;
        }

      case State.OFF_TABLE:
        {
          if (firstFrameAfterFall)
          {
            firstFrameAfterFall = false;
            return;
          }

          foreach (uint id in InternalCalls.GetAllChildren(brokenOrb.mEntityID))
          {
            InternalCalls.LockRigidBody(id, false);
          }

          currState = State.INACTIVE;
          //Destroy(this);  // we are done here

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

    Vector3 zeroes = Vector3.Zero;
    InternalCalls.SetRotationEuler(mEntityID, ref zeroes);
    InternalCalls.SetVelocity(mEntityID, zeroes);
    InternalCalls.SetAngularVelocity(mEntityID, zeroes);
    InternalCalls.SetGravityFactor(mEntityID, 20f);

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