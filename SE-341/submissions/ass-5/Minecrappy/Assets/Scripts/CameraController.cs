using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CameraController : MonoBehaviour
{
    public float rSpeed = 3.0f;

    public float X = 0.0f;
    public float Y = 0.0f;

    public Transform playerTransform;
    private Vector3 deviation;

    //private Rigidbody rb;

    // Start is called before the first frame update
    void Start()
    {
        deviation = transform.position - playerTransform.position;
    }

    // Update is called once per frame
    void Update()
    {

        //X += Input.GetAxis("Mouse X") * rSpeed * 2;
        //Y += Input.GetAxis("Mouse Y") * rSpeed;
        //transform.localRotation = Quaternion.AngleAxis(X, Vector3.up);
        //transform.localRotation *= Quaternion.AngleAxis(Y, Vector3.left);

        //transform.position = playerTransform.position + deviation;



        float cam_h = Input.GetAxis("Mouse X") * rSpeed;
        // inverse y axis
        float cam_v = -Input.GetAxis("Mouse Y") * rSpeed;

        if (KeyboardController.isPaused())
        {
            cam_h = 0f;
            cam_v = 0f;
        }

        transform.position = deviation + playerTransform.position;

        transform.RotateAround(playerTransform.position, Vector3.up, cam_h * 0.5f);

        float angleX = transform.rotation.eulerAngles.x;
        float nextAngleX = cam_v * 0.5f + angleX;

        if (nextAngleX >= 360f)
        {
            nextAngleX -= 360f;
        }

        if ((nextAngleX < 80f && nextAngleX > 0f) || (nextAngleX <= 360f && nextAngleX >= 280f))
        {
            transform.RotateAround(playerTransform.position, transform.right, cam_v * 0.5f);
        }
        deviation = transform.position - playerTransform.position;
    }
}