using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlayerController : MonoBehaviour
{
    Rigidbody2D rb;

    Animator animator;

    // character horizontal moving speed
    public float speed;

    // character vertical jumping speed
    public float jumpSpeed;

    // min jump speed before next jump
    public float minJumpSpeed;

    private float speedDecelerator = 0.4f;

    private float sigma = 0.01f;

    private float maxMoveSpeed = 5.0f;

    public float checkRadius;
    public LayerMask platformMask;
    public GameObject checkPoint;
    public bool isOnGround;

    // Start is called before the first frame update
    void Start()
    {
        rb = GetComponent<Rigidbody2D>();

        animator = GetComponent<Animator>();
    }

    // Update is called once per frame
    void Update()
    {
        isOnGround = Physics2D.OverlapCircle(checkPoint.transform.position,
checkRadius, platformMask);
        animator.SetBool("isOnGround", isOnGround);

        Move();
    }

    void Move()
    {
        float xInput = Input.GetAxisRaw("Horizontal");
        float yInput = Input.GetAxisRaw("Vertical");

        float xSpeed = rb.velocity.x + xInput;

        if (System.Math.Abs(xSpeed) <= sigma)
		{
            xSpeed = 0.0f;
		} else if (xSpeed > 0.0f)
		{
            xSpeed -= speedDecelerator;
        } else if (xSpeed < 0.0f)
		{
            xSpeed += speedDecelerator;
		}

        if (xSpeed > maxMoveSpeed)
		{
            xSpeed = maxMoveSpeed;
		} else if (xSpeed < -maxMoveSpeed)
		{
            xSpeed = -maxMoveSpeed;
		}

        if (yInput == 1 && isOnGround)
        {
            rb.velocity = new Vector2(xSpeed, jumpSpeed);
        } else
		{
            rb.velocity = new Vector2(xSpeed, rb.velocity.y);
        }

        if (xInput != 0)
        {
            transform.localScale = new Vector3(xInput, 1, 1);
        }

        animator.SetFloat("speed", System.Math.Abs(xSpeed));
    }

    void OnTriggerEnter2D(Collider2D other)
	{
        if (other.CompareTag("Spike"))
		{
            Die("hitting a spike");
		}
	}

    void OnCollisionEnter2D(Collision2D other)
	{
        if (other.gameObject.CompareTag("SpikeBall"))
		{
            Die("hitting a spike ball");
		} else if (other.gameObject.CompareTag("WorldEdge"))
		{
            Die("falling beyond the world");
		} else if (other.gameObject.CompareTag("Platform"))
		{
            GameMaster.instance.MakeScoreBonus(100);
        }
	}

    private void OnDrawGizmosSelected()
    {
        Gizmos.color = Color.blue;
        Gizmos.DrawWireSphere(checkPoint.transform.position, checkRadius);
    }

    public void Die(string reason)
	{
        // Debug.Log(string.Format("You died because of {0}!", reason));
        GameMaster.GameOver(string.Format("You died because of {0}!", reason));
	}
}
