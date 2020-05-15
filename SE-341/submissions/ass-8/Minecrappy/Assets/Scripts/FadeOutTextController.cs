using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class FadeOutTextController : MonoBehaviour
{
    public TMPro.TextMeshPro tmp;
    // Start is called before the first frame update
    void Start()
    {
        tmp = GetComponent<TMPro.TextMeshPro>();
    }

    // Update is called once per frame
    void Update()
    {
        var alpha = tmp.color.a - Time.deltaTime;
        if (alpha < 0) {
            Destroy(gameObject);
        }
        tmp.color = new Color(tmp.color.r, tmp.color.g, tmp.color.b, alpha);
    }
}
