using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class StarMotionController : MonoBehaviour
{
    public float rotationSpeed = 0.3f;

    public float defaultSpeedX;
    public float defaultSpeedY;

    public int offset;

    private Vector3 speed;
    private float scale = 0.002f;

    // Start is called before the first frame update
    void Start()
    {
        var random = new System.Random();

        var offsetX = System.Convert.ToSingle(random.Next(offset) - offset) / 2f;
        var offsetY = System.Convert.ToSingle(random.Next(offset) - offset) / 2f;

        speed = new Vector3(defaultSpeedX + offsetX, defaultSpeedY + offsetY, 0);
    }

    // Update is called once per frame
    void Update()
    {
        transform.position += speed * scale;
        transform.localEulerAngles += new Vector3(0, 0, rotationSpeed);
    }
}
