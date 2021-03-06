﻿using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PendulumTypeAController : MonoBehaviour
{
    public GameObject anchor;
    public Vector3 faceVector;

    public float gValue = 9.80665f;

    private float distance;
    private float theta;
    private float omega;

    private Vector3 upVector = new Vector3(0, 1, 0);
    private Vector3 targetVector;

    private LineRenderer line;

    // Start is called before the first frame update
    void Start()
    {
        Vector3 distanceVector = anchor.transform.position - gameObject.transform.position;

        distance = distanceVector.magnitude;
        theta = Vector3.Angle(distanceVector, upVector) * Mathf.Deg2Rad;
        omega = 0;

        targetVector = Vector3.Cross(upVector, faceVector).normalized;

        // Debug.Log(string.Format("init! distance: {0}, theta (deg): {1}, omega: {2}\n", distance, theta * Mathf.Rad2Deg, omega));
        line = gameObject.AddComponent<LineRenderer>();
        line.material = new Material(Shader.Find("UI/Default"));
        line.positionCount = 2;
        line.startColor = Color.red;
        line.endColor = Color.blue;
        line.startWidth = 2f;
        line.endWidth = 2f;
    }

    // Update is called once per frame
    void Update()
    {
        // Debug.Log(string.Format("update! distance: {0}, theta (deg): {1}, omega: {2}\n", distance, theta * Mathf.Rad2Deg, omega));
        theta += omega * Time.deltaTime;
        omega -= gValue / distance * (float)System.Math.Sin(theta) * Time.deltaTime;

        gameObject.transform.position = anchor.transform.position + distance * (-upVector * (float)System.Math.Cos(theta) + targetVector * (float)System.Math.Sin(theta));
        line.SetPosition(0, gameObject.transform.position);
        line.SetPosition(1, anchor.transform.position);
    }
}
