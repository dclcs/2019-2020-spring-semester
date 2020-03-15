using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class BackgroundOffsetManager : MonoBehaviour
{
    // background moving speed: (v_x, v_y)
    public Vector2 speed;

    // background material
    Material material;

    // actual location
    Vector2 movement;

    // Start is called before the first frame update
    void Start()
    {
        material = GetComponent<Renderer>().material;
    }

    // Update is called once per frame
    void Update()
    {
        movement += speed;
        material.mainTextureOffset = movement;
    }
}
