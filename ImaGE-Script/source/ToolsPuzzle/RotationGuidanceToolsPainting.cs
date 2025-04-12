using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Numerics;
using IGE.Utils;

public class RotationGuidanceToolsPainting : Entity
{
  public Transform player;
  public Entity mainCamera;

  public Entity leftArrow;
  public Entity rightArrow;
  public Entity topArrow;
  public Entity bottomArrow;

  public PictureAlignTools pictureAlign;
  public float rotationThreshold = 2f;

  void Start()
  {
    DisableAllArrows();
  }

  void Update()
  {

    if (!pictureAlign.isFrozen && !pictureAlign.alignCheck)
    {
      GuideRotation();
    }
    else
    {
      DisableAllArrows();
    }
  }

  void GuideRotation()
  {
    Quaternion currentCameraRotation = mainCamera.GetComponent<Transform>().rotation;
    Quaternion savedCameraRotation = pictureAlign.GetSavedCameraRotation();

    //Vector3 currentCameraEulerAngles = Mathf.QuatToEuler(currentCameraRotation);
    //Vector3 savedCameraEulerAngles = Mathf.QuatToEuler(savedCameraRotation);

    //float xDifference = Mathf.DeltaAngle(currentCameraEulerAngles.X, savedCameraEulerAngles.X);
    //float yDifference = Mathf.DeltaAngle(currentCameraEulerAngles.Y, savedCameraEulerAngles.Y);

   // CheckCameraRotation(xDifference, yDifference);
  }

  void CheckCameraRotation(float xDifference, float yDifference)
  {
    DisableAllArrows();

    if (Mathf.Abs(yDifference) > rotationThreshold)
    {
      if (yDifference > 0)
      {
        rightArrow.SetActive(true);
      //Debug.Log("Right arrow enabled.");
      }
      else
      {

        leftArrow.SetActive(true);
      //Debug.Log("Left arrow enabled.");
      }
    }

    if (Mathf.Abs(xDifference) > rotationThreshold)
    {
      if (xDifference > 0)
      {

        bottomArrow.SetActive(true);
      //Debug.Log("Top arrow enabled.");
      }
      else
      {

        topArrow.SetActive(true);
      //Debug.Log("Bottom arrow enabled.");
      }
    }
  }

  void DisableAllArrows()
  {
    leftArrow.SetActive(false);
    rightArrow.SetActive(false);
    topArrow.SetActive(false);
    bottomArrow.SetActive(false);
  }
}
