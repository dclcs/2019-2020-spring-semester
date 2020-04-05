using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlayerController : MonoBehaviour
{

    private Rigidbody rb;

    public Vector3 speed;

    public float mSpeed = 500.0f;

    public Camera cam;

    // Start is called before the first frame update
    void Start()
    {
        rb = GetComponent<Rigidbody>();
    }

    // Update is called once per frame
    void Update()
    {
        var X = Input.GetAxis("Vertical");
        var Y = Input.GetAxis("Horizontal");

        var cam_y = cam.transform.rotation.eulerAngles.y;
        var force = Quaternion.Euler(0, cam_y, 0) * new Vector3(Y, 0, X) * mSpeed * Time.deltaTime;
        rb.AddForce(force);
    }
}
