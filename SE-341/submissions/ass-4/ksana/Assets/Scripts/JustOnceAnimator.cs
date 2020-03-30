using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class JustOnceAnimator : MonoBehaviour
{

    // Start is called before the first frame update
    void Start()
    {
        GetComponent<Animator>().SetTrigger("OnceTrigger");
    }

    // Update is called once per frame
    void Update()
    {
        
    }
}
