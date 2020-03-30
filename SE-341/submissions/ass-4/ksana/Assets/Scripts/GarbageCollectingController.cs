using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class GarbageCollectingController : MonoBehaviour
{

    const int maxLUsize = 10;
    const int maxRDsize = 20;

    // Start is called before the first frame update
    void Start()
    {

    }

    // Update is called once per frame
    void Update()
    {
        var position = transform.position;
        if ((position.y > maxLUsize || position.y < -maxRDsize)
            && (position.x > maxRDsize || position.x < -maxLUsize)) {
            GameSupervisor.instance.elementCount--;
            Destroy(gameObject);
        }
    }
}
