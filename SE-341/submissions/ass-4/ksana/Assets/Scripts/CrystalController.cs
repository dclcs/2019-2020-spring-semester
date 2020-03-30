using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CrystalController : MonoBehaviour
{

    Animator animator;

    bool alreadyExplodes = false;
    double timer = 0.0;


    // Start is called before the first frame update
    void Start()
    {
        animator = GetComponent<Animator>();
    }

    // Update is called once per frame
    void Update()
    {
        if (alreadyExplodes)
		{
            timer -= Time.deltaTime;

            if (timer <= 0.0)
			{
                Destroy(gameObject);
			}
        }
    }

    void OnTriggerEnter2D(Collider2D other)
    {
        if (other.gameObject.CompareTag("Player"))
        {
            GetComponent<Animator>().SetTrigger("CollectTrigger");

            timer = 0.6;
            alreadyExplodes = true;
        }
    }
}
