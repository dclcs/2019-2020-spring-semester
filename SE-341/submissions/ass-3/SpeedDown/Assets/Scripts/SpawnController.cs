using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class SpawnController : MonoBehaviour
{
    public float spawnTime;

    private float countTime;

    public List<GameObject> platforms;
    
    // Start is called before the first frame update
    void Start()
    {
        if (spawnTime == 0.0f)
		{
            spawnTime = 3.0f;
		}
    }

    // Update is called once per frame
    void Update()
    {

        countTime += Time.deltaTime;

        if (countTime >= spawnTime)
		{
            SpawnPlatform();
            countTime = 0.0f;
		}
    }

    public void SpawnPlatform()
	{
        Vector3 spawnPosition = transform.position;

        System.Random random = new System.Random();
        spawnPosition.x = System.Convert.ToSingle(random.Next() % 7) - 3.5f;

        int index = random.Next() % platforms.Count;

        GameObject go = Instantiate(platforms[index], spawnPosition, Quaternion.identity);

        go.transform.SetParent(this.gameObject.transform);
	}
}
