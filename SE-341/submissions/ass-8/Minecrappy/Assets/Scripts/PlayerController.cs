using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlayerController : MonoBehaviour
{

    private Rigidbody rb;

    public Vector3 speed;

    public float mSpeed = 0f;

    public float jumpSpeed = 0f;

    public float flySpeed = 0f;
    
    public Camera cam;

    private static bool grounded = false;

    // Start is called before the first frame update
    void Start()
    {
        rb = GetComponent<Rigidbody>();
    }

    // Update is called once per frame
    void Update()
    {
        if (KeyboardController.isPaused())
		{
            return;
		}
        var X = Input.GetAxis("Vertical");
        var Y = Input.GetAxis("Horizontal");

        var cam_y = cam.transform.rotation.eulerAngles.y;
        var force = Quaternion.Euler(0, cam_y, 0) * new Vector3(Y, 0, X) * mSpeed * Time.deltaTime;
        rb.AddForce(force);

        if (Input.GetButtonUp("Jump"))
		{
            Jump();
		}

        if (Input.GetButton("Fly"))
        // if (Input.G(KeyCode.LeftShift))
		{
            Fly();
        }
    }

    private void Jump()
	{
        if (PlayerController.grounded)
        {
            rb.AddForce(new Vector3(0f, jumpSpeed, 0f));
			PlayerController.grounded = false;
		}
    }

    private void Fly()
	{
        rb.AddForce(new Vector3(0f, flySpeed * Time.deltaTime, 0f));
        PlayerController.grounded = false;
    }

    private void OnTriggerEnter(Collider other)
    {
        PlayerController.grounded = true;
    }

	//private void OnTriggerExit(Collider other)
	//{
	//    PlayerController.grounded = false;
	//}

	public static bool IsOnGround()
	{
        return PlayerController.grounded;
	}
}
