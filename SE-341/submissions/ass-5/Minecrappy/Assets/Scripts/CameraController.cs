using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CameraController : MonoBehaviour
{
    public float rSpeed = 3.0f;
    public float mSpeed = 20.0f;
    public float X = 0.0f;
    public float Y = 0.0f;

    private float ySpeed = 0.0f;

    //private Rigidbody rb;

    // Start is called before the first frame update
    void Start()
    {
        //rb = GetComponent<Rigidbody>();
    }

    // Update is called once per frame
    void Update()
    {
        X += Input.GetAxis("Mouse X") * rSpeed * 2;
        Y += Input.GetAxis("Mouse Y") * rSpeed;
        transform.localRotation = Quaternion.AngleAxis(X, Vector3.up);
        transform.localRotation *= Quaternion.AngleAxis(Y, Vector3.left);
        transform.position += transform.forward * mSpeed * Input.GetAxis("Vertical") * Time.deltaTime;
        transform.position += transform.right * mSpeed * Input.GetAxis("Horizontal") * Time.deltaTime;

        var pos = transform.position;

        ySpeed += 0.048f;
        pos.y -= ySpeed;


        if (pos.y <= 1)
        {
            pos.y = 1;
            ySpeed = 0;
        }


        transform.position = pos;

		if (Input.GetKeyDown(KeyCode.Space))
		{
            ySpeed = -0.5f;
		}

		//var locR = transform.localEulerAngles;

		//Debug.Log(locR);

		//float angle = locR.x - 180;

		//if (angle > 0)
		//{
		//    locR.x = angle - 180;
		//}
		//else
		//{
		//    locR.x = angle + 180;
		//}

		//Debug.Log(string.Format("fixed {0}", locR));

		//transform.localEulerAngles = new Vector3(locR.x, locR.y, locR.z);
	}
}