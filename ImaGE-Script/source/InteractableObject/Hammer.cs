//using System.Collections;
//using System.Collections.Generic;
//using UnityEngine;

//public class Hammer : Entity, IInventoryItem
//{
//    public string Name
//    {
//        get
//        {
//            return "Hammer";
//        }
//    }

//    public Sprite _Image = null;

//    public Sprite Image
//    {
//        get
//        {
//            return _Image;
//        }
//    }


//    public void OnPickup()
//    {
//        Entity.SetActive(false);

//    }

//    public void OnUsed()
//    {
//        Destroy(Entity);
//    }
//}