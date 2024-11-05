using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;
using System.Xml;
using Image.Utils;


public class FinishLineTrigger : Entity
{
  public bool crossFinish = false;
  // Start is called before the first frame update
  private void OnTriggerEnter(Collider other)
  {
    if (other.CompareTag("Player"))
    {
      crossFinish = true;
      Debug.Log("crossed");
    }
  }
}

