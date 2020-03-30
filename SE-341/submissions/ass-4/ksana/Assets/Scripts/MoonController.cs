using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class MoonController : MonoBehaviour
{
    private float forceConst = 0.01f;

    private float scaleIndex = 0.1f;

    Rigidbody2D moonObject;
    // Start is called before the first frame update
    void Start()
    {
        var random = new System.Random();

        moonObject = GetComponent<Rigidbody2D>();

        var velX = -System.Convert.ToSingle(random.Next(4)) - 2.0f;
        var velY = System.Convert.ToSingle(random.Next(5)) + 1.0f;
        moonObject.velocity = new Vector3(velX, velY, 0) * scaleIndex;
    }

    // Update is called once per frame
    void Update()
    {
        var rb = GameSupervisor.instance.player;

        var mag = System.Math.Max((rb.position - moonObject.position).magnitude, 0.01f);

        var force = forceConst * (rb.position - moonObject.position).normalized / (mag * mag);

        //Debug.Log(string.Format("Force: {0}", force));
        rb.velocity += force;
        moonObject.velocity -= force;
    }
}