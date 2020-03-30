using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class FadePromptIconController : MonoBehaviour
{
    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    { 
        Renderer r = gameObject.GetComponent<Renderer>();
        Color newColor = r.material.color;
        if (newColor.a > 0.003f)
        {
            newColor.a -= 0.003f;
            r.material.color = newColor;
        } else
		{
            Destroy(gameObject);
		}
    }
}
