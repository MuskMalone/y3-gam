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

public class GlowingLight : Entity
{
  public Entity BloomObject;
  public float duration;
  public float startIntensity, targetIntensity;

  private float timeElapsed = 0f;
  private bool started = false;

  // Start is called before the first frame update
  void Start()
  {

  }

  // Update is called once per frame
  void Update()
  {
    if (!started) { return; }

    timeElapsed += Time.deltaTime;

    if (timeElapsed >= duration)
    {
      InternalCalls.SetBloomIntensity(BloomObject.mEntityID, targetIntensity);
      Destroy(this);    
    }

    float t = timeElapsed / duration;
    InternalCalls.SetBloomIntensity(BloomObject.mEntityID, startIntensity + t * (targetIntensity - startIntensity));
  }

  public void StartBlooming()
  {
    started = true;
  }
}
