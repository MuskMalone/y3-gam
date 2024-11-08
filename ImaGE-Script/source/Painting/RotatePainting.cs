//using System;
//using System.Collections.Generic;
//using System.Linq;
//using System.Text;
//using System.Threading.Tasks;
//using System.Numerics;
//using Image.Utils;

//public class RotatePainting : Entity
//{
//    // Public field to drag and drop the UI element (RectTransform or Image) in the Inspector
//    public RectTransform painting;
//    public RectTransform border;

//    // Update is called once per frame
//    void Update()
//    {
//        if (Input.GetKeyDown(KeyCode.R))
//        {
//            // Rotate the UI element by 90 degrees around the Z-axis
//            if (painting != null && border != null)
//            {
//                painting.Rotate(0, 0, 45);
//                border.Rotate(0, 0, 45);
//            }
//        }
//    }
//}
