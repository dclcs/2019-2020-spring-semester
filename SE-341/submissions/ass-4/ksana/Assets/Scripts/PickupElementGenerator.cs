using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PickupElementGenerator : MonoBehaviour
{
    public List<GameObject> elements;

    public float spawnTime;
    private float countTime;

    // Start is called before the first frame update
    void Start()
    {
        if (spawnTime == 0.0f)
        {
            spawnTime = 5.0f;
        }

        countTime = System.Convert.ToSingle(new System.Random().Next(System.Convert.ToInt32(spawnTime)));
    }

    // Update is called once per frame
    void Update()
    {
        if (!GameSupervisor.instance.isInGame)
        {
            return;
        }
        countTime += Time.deltaTime;

        if (countTime >= spawnTime)
        {
            spawnElements();
            countTime = 0.0f;
        }
    }

    public void spawnElements()
    {
        if (elements.Count == 0 || GameSupervisor.instance.elementCount >= GameSupervisor.maxElementCount)
        {
            return;
        }
        var random = new System.Random();

        Vector3 spawnPosition = transform.position;

        spawnPosition.x += System.Convert.ToSingle(random.Next() % 7) - 3.5f;
        //spawnPosition.z += (System.Convert.ToSingle(random.Next() % 7) - 3.5f) / 4f;

        int index = random.Next() % elements.Count;

        GameObject go = Instantiate(elements[index], spawnPosition, Quaternion.identity);

        Vector3 rotation = new Vector3(0, 0, random.Next() % 360);

        go.transform.Rotate(rotation);
        go.transform.SetParent(this.gameObject.transform);


        var scaleFactor = System.Convert.ToSingle(random.Next(10)) / 10f + 0.5f;
        go.transform.localScale *= scaleFactor;

        GameSupervisor.instance.elementCount++;
    }
}
