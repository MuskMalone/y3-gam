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
  public Entity tableSurface, hexFloor;

  private static readonly float violentDuration = 5f;
  private static readonly float riseVelocity = 4f;
  private static readonly float intensity = 0.002f; // How much it vibrates
  private static readonly float frequency = 0.05f;  // How fast it vibrates
  private static readonly float violentIntensity = intensity * 3f; // How much it vibrates
  private static readonly float violentFrequency = frequency * 0.3f;  // How fast it vibrates
  private static readonly Vector3 tableSurfaceMidpoint = new Vector3(63.429f, 67.564f, -425.342f);
  private static readonly float pushOffStrength = 45f;

  private LeverManager leverManager;

  public enum State
  {
    INACTIVE,
    RISING,
    HOVERING,
    VIOLENT,
    FALLING,
    ROLLING,
    OFF_TABLE,
    SHATTERED
  }

  private uint firstBrokenPiece;
  private State currState = State.INACTIVE;
  private Vector3 originalPos;
  private float timeElapsed = 0f, violentTimer = 0f;
  private bool firstFrameAfterFall = true;

  public float delayBeforeLightPingSFX = 0.472f;
  public float timeTakenToGlow = 3.8f;
  public float timer = 0f;
  private bool toPlay = true;
  // Start is called before the first frame update
  void Start()
  {
    timer = 0f;
    InternalCalls.SetSoundVolume(mEntityID, "Spin", 0);
    InternalCalls.SetSoundVolume(mEntityID, "SpinFaster", 0);
    leverManager = FindObjectOfType<LeverManager>();

    StopGlowing();
    brokenOrb.SetActive(false);
    leverManager.AddOrb(this);  // give leverManager a ref
    firstBrokenPiece = InternalCalls.GetAllChildren(brokenOrb.mEntityID)[0];
  }

  // Update is called once per frame
  void Update()
  {
    switch (currState)
    {
      case State.RISING:
        {
          timer += InternalCalls.GetDeltaTime();
          if (timer >= timeTakenToGlow - delayBeforeLightPingSFX && toPlay == true)
          {
            toPlay = false;
            InternalCalls.SetSoundVolume(mEntityID, "Light", 3f);
            InternalCalls.PlaySound(mEntityID, "Light");
          }
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
          InternalCalls.SetSoundVolume(mEntityID, "Spin", 0.4f);
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
          InternalCalls.SetSoundVolume(mEntityID, "SpinFaster", 0.5f);

          timeElapsed += Time.deltaTime;
          violentTimer += Time.deltaTime;

          if (timeElapsed >= violentDuration)
          {
            Fall();
            InternalCalls.StopSound(mEntityID, "Spin");
            InternalCalls.StopSound(mEntityID, "SpinFaster");
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
            InternalCalls.SetSoundVolume(mEntityID, "Roll", 0.5f);
            InternalCalls.PlaySound(mEntityID, "Roll");
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
            InternalCalls.StopSound(mEntityID, "Roll");

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
          currState = State.SHATTERED;
          
          break;
        }

      case State.SHATTERED:
        {
          // if pieces touched the ground
          if (InternalCalls.GetContactPoints(firstBrokenPiece, hexFloor.mEntityID).Length > 0)
          {

            leverManager.OrbShattered();
            
            InternalCalls.PlaySound(mEntityID, "Break");
            Destroy();  // no longer needs this entity
          }

          break;
        }

      default:
        return;
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