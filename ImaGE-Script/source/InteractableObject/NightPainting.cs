using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class NightPainting : Entity, IInventoryItem
{
    public string Name
    {
        get
        {
            return "NightPainting";
        }
    }

    public Sprite _Image = null;

    public Sprite Image
    {
        get
        {
            return _Image;
        }
    }

    //public void OnPickup()
    //{
    //    Destroy(Entity);
    //}

    public void OnPickup()
    {
        //Destroy(Entity); 
        Entity.SetActive(false);

    }

    public void OnUsed()
    {
        Destroy(Entity);
    }
}
