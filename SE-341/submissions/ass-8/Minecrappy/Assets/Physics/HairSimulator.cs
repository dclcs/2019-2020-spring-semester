using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class HairNode {
    public Vector3 position;
    public Vector3 lastPosition;
    public float length;
}

public class HairSimulator : MonoBehaviour
{

    public Vector3 hairRoot;
    public float damping;
    public int iterationCount;
    public int nodeCount;
    public float hairLength;
    public float hairThickness;
    public Color hairColor = Color.white;

    private List<HairNode> Nodes = new List<HairNode>();
    private LineRenderer line;
    private Rigidbody rigidbody;
    private Vector3 currentAcceleration;
    private Vector3 previousVelocity, currentVelocity;

    public const float FixedUpdateTimeStep = 0.02f;

    // calculates every 0.02f seconds
    void FixedUpdate() {
        previousVelocity = currentVelocity;
        currentVelocity = rigidbody.velocity;
        currentAcceleration = new Vector3((currentVelocity - previousVelocity).x / FixedUpdateTimeStep,
                                   (currentVelocity - previousVelocity).y / FixedUpdateTimeStep,
                                   (currentVelocity - previousVelocity).z / FixedUpdateTimeStep);
    }

    // Start is called before the first frame update
    void Start()
    {
        rigidbody = GetComponent<Rigidbody>();

        for (int i = 0; i < nodeCount; i++)
        {
            HairNode node = new HairNode();
            node.length = hairLength;
            Nodes.Add(node);
        }

        GameObject lineDrawer = new GameObject();
        line = lineDrawer.AddComponent<LineRenderer>();
        line.material = new Material(Shader.Find("UI/Default"));
        line.positionCount = nodeCount;
        line.startColor = hairColor;
        line.endColor = hairColor;
        line.startWidth = hairThickness;
        line.endWidth = hairThickness / 2;
    }

    // Update is called once per frame
    void Update()
    {
        foreach (var node in Nodes)
        {
            Vector3 newPosition = Verlet(node.lastPosition, node.position, damping, currentAcceleration, Time.deltaTime);
            node.lastPosition = node.position;
            node.position = newPosition;
        }

        for (int iter = 0; iter < iterationCount; iter++)
        {
            for (int i = 0; i < Nodes.Count - 1; i++)
            {
                HairNode nodeA = Nodes[i], nodeB = Nodes[i + 1];
                collideBox(GetComponent<SphereCollider>(), ref nodeB.position);

                lengthConstraint(nodeA.position, ref nodeB.position, nodeB.length);
            }
            Nodes[0].position = hairRoot;
        }

        for (int i = 0; i < Nodes.Count; i++) {
            line.SetPosition(i, Nodes[i].position);
        }
    }

    Vector3 Verlet(Vector3 lastPosition, Vector3 position, float damping, Vector3 acceleration, float deltaTime) {
        acceleration += new Vector3(0, -9.80665f, 0);
        return position + (position - lastPosition) * damping + acceleration * deltaTime * deltaTime;
    }

    void collideBox(SphereCollider collider, ref Vector3 position) {
        Vector3 colliderCenter = gameObject.transform.position + collider.center;
        Vector3 diff = colliderCenter - position;
        if (diff.magnitude < collider.radius) {
            position = colliderCenter + diff.normalized * collider.radius;
        }
    }

    void lengthConstraint(Vector3 startPosition, ref Vector3 endPosition, float length) {
        Vector3 diff = (endPosition - startPosition).normalized * length;
        endPosition = startPosition + diff;
    }
}
