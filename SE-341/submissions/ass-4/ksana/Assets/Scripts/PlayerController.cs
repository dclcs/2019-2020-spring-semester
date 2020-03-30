using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlayerController : MonoBehaviour
{

    Rigidbody2D rb;

    public float sensitivity;

    public AudioClip breakAudioClip;

    // Start is called before the first frame update
    void Start()
    {
        rb = GetComponent<Rigidbody2D>();

        GameSupervisor.instance.player = rb;
    }

    // Update is called once per frame
    void Update()
    {
        if (GameSupervisor.instance.isInGame)
        {
            Move();
        } else
		{
            rb.velocity = new Vector2(0, 0);
		}
    }

    private void FixedUpdate()
    {
        if (rb.bodyType == RigidbodyType2D.Dynamic)
        {
            var v2 = rb.velocity;

            var rotation = System.Convert.ToSingle(System.Math.Atan2(v2.y + 5, v2.x) * 180f / System.Math.PI - 90f);
            //Debug.Log(string.Format("rotation v: {0}; deltaTime: {1}, velocity: {2}, {3}", rotation, deltaTime, v2.x, v2.y));
            rb.rotation = rotation;
        }
    }

    void Move()
	{
        float xInput = Input.GetAxisRaw("Horizontal");
        float yInput = Input.GetAxisRaw("Vertical");

        var velocity = rb.velocity;

        velocity.x += xInput * sensitivity;
        velocity.y += yInput * sensitivity;

        rb.velocity = velocity;
    }

    void OnTriggerEnter2D(Collider2D other)
	{
        if (other.gameObject.CompareTag("Crystal"))
		{
            GameSupervisor.instance.crystalCount++;
            AudioSource.PlayClipAtPoint(breakAudioClip, transform.localPosition);
        }
	}
}
