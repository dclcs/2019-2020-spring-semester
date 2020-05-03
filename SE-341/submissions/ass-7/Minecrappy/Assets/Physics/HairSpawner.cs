using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class HairSpawner : MonoBehaviour
{
    public float damping;
    public int iterationCount = 10;
    public int nodeCount = 10;
    public float hairLength = 0.1f;
    public float hairThickness = 0.001f;
    public Color hairColor;

    private List<HairSimulator> simulators = new List<HairSimulator>();
    private float radius;

    // Start is called before the first frame update
    void Start()
    {
        radius = GetComponent<SphereCollider>().radius;
        
        for (int i = 0; i < 100; ++i) {
            HairSimulator simulator = gameObject.AddComponent<HairSimulator>();
            simulator.damping = damping;
            simulator.iterationCount = iterationCount;
            simulator.nodeCount = nodeCount;
            simulator.hairLength = hairLength;
            simulator.hairThickness = hairThickness;
            simulator.hairColor = hairColor;
            simulators.Add(simulator);
        }
    }

    // Update is called once per frame
    void Update() {
        for (int theta = 0; theta < 10; theta += 1)
        {
            for (int phi = 0; phi < 10; phi += 1)
            {
                float thetaF = (theta * 5 + transform.eulerAngles.y) * Mathf.Deg2Rad;
                float phiF = (phi * 5 + transform.eulerAngles.x) * Mathf.Deg2Rad;
                // Debug.Log(string.Format("Handling {0}", theta * 7 + phi));
                HairSimulator sim = simulators[theta * 10 + phi];
                Vector3 relativePos = new Vector3(radius * Mathf.Sin(thetaF) * Mathf.Cos(phiF), radius * Mathf.Sin(thetaF) * Mathf.Sin(phiF), radius * Mathf.Cos(thetaF));
                // Debug.Log(relativePos);
                sim.hairRoot = gameObject.transform.position + relativePos;
            }
        }
    }
}
