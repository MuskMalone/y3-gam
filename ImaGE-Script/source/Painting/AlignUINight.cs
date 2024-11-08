//using System;
//using System.Collections.Generic;
//using System.Linq;
//using System.Text;
//using System.Threading.Tasks;
//using System.Numerics;
//using IGE.Utils;

//public class AlignUINight : Entity
//{
//    private PictureAlignNight alignScript;  
//    private TextMeshProUGUI textComponent;  
//    private bool isAlignScriptDestroyed = false;  

//    void Start()
//    {
//        alignScript = FindObjectOfType<PictureAlignNight>();

//        textComponent = GetComponent<TextMeshProUGUI>();

//        textComponent.enabled = false;
//    }

//    void Update()
//    {
//        if (isAlignScriptDestroyed || alignScript == null)
//        {

//            textComponent.enabled = false;
//            return;
//        }

//        if (alignScript != null)
//        {
 
//            if (alignScript.isFrozen)
//            {
//                textComponent.enabled = false;
//            }
//            else
//            {
 
//                if (alignScript.alignCheck)
//                {
//                    textComponent.enabled = true;
//                }
//                else
//                {
//                    textComponent.enabled = false;
//                }
//            }
//        }
//        else
//        {
//            Debug.LogError("alignScript reference is missing.");

//            isAlignScriptDestroyed = true;

//            textComponent.enabled = false;
//        }
//    }
//}
