using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ForegroundShakingController : MonoBehaviour
{

    // 抖动目标的 transform
    public Transform camTransform;

    // 抖动幅度
    public float shakeScale = 10000.0f;

    Vector3 originalPos;
    Vector3 moveSpeed = new Vector3(0.0f, 0.0f, 0.0f);

    public float maxOffset = 0.5f;

    private float minOffset = 0.3f;

    private float maxSpeed = 0.2f;

    private int counter = 0;

    private int maxCount = 10;

    void OnEnable()
    {
        originalPos = camTransform.localPosition;

        var random = new System.Random();

        var rand1 = System.Convert.ToSingle(random.Next(0, 10)) / 10f - 0.5f;
        var rand2 = System.Convert.ToSingle(random.Next(0, 10)) / 10f - 0.5f;
        var rand3 = System.Convert.ToSingle(random.Next(0, 10)) / 10f - 0.5f;

        moveSpeed += new Vector3(rand1 / shakeScale, rand2 / shakeScale, rand3 / shakeScale);
    }

    void Awake()
    {
        if (camTransform == null)
        {
            camTransform = GetComponent(typeof(Transform)) as Transform;
        }
    }

    void Update()
    {

        if (counter == maxCount)
        {

            var offset = originalPos - camTransform.localPosition;

            if (System.Math.Abs(offset.x) > minOffset || System.Math.Abs(offset.y) > minOffset || System.Math.Abs(offset.z) > minOffset)
            {
                moveSpeed += offset / maxOffset;
            }
            else
            {
                var random = new System.Random();

                var rand1 = System.Convert.ToSingle(random.Next(0, 10)) / 10f - 0.5f;
                var rand2 = System.Convert.ToSingle(random.Next(0, 10)) / 10f - 0.5f;
                var rand3 = System.Convert.ToSingle(random.Next(0, 10)) / 10f - 0.5f;

                moveSpeed += new Vector3(rand1 / shakeScale, rand2 / shakeScale, rand3 / shakeScale);

                moveSpeed.x = System.Math.Min(maxSpeed, System.Math.Max(moveSpeed.x, -maxSpeed));
                moveSpeed.y = System.Math.Min(maxSpeed, System.Math.Max(moveSpeed.y, -maxSpeed));
                moveSpeed.z = System.Math.Min(maxSpeed, System.Math.Max(moveSpeed.z, -maxSpeed));
                //Debug.Log(string.Format("Current Position: {0}", camTransform.localPosition));
                //Debug.Log(string.Format("Offset Position: {0}", offset));
                //Debug.Log(string.Format("Current Speed: {0}", moveSpeed));

            }
            counter = 0;
        } else
		{
            ++counter;
		}

        camTransform.localPosition += moveSpeed / maxCount;
    }
}
