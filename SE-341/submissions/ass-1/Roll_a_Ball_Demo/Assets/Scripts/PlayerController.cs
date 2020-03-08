using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class PlayerController : MonoBehaviour
{

    public float speed = 500.0f;
    private int count;
    public Text countText;

    // Start is called before the first frame update
    void Start()
    {
        setCountText();
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    private void FixedUpdate()
	{
        float moveHorizontal = Input.GetAxis("Horizontal");
        float moveVertical = Input.GetAxis("Vertical");

        Vector3 movement = new Vector3(moveHorizontal, 0.0f, moveVertical);

        GetComponent<Rigidbody>().AddForce(movement * speed * Time.deltaTime);
	}

    private void OnTriggerEnter(Collider other)
	{
        if (other.gameObject.tag == "Pickup")
		{
            other.gameObject.SetActive(false);
            count++;
            setCountText();
        }
	}

    private void setCountText()
	{
        countText.text = string.Format("Count: {0}", count);
	}
}
