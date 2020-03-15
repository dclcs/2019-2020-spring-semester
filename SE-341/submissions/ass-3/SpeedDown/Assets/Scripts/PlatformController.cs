using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlatformController : MonoBehaviour
{
    // platform uprising speed
    public float speed;

    // platform y axis limit
    GameObject topLine;

    // platform speed vector
    Vector3 speedVec3;


    // Start is called before the first frame update
    void Start()
    {
        speedVec3.y = speed;

        topLine = GameObject.Find("TopLine");
    }

    // Update is called once per frame
    void Update()
    {
        Move();   
    }

    void Move()
	{
        transform.position += speedVec3 * Time.deltaTime;

        if (transform.position.y < -10.0f)
		{
            Destroy(gameObject);
		}
	}
}
