using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class GameSpawnController : MonoBehaviour
{
    public GameObject blockPrefab;
    public GameObject spherePrefab;

    private System.Random random = new System.Random();

    public float refreshTime = 3;
    public int maxBlockCount = 20;
    private float currentTime;

    // Start is called before the first frame update
    void Start()
    {
        currentTime = refreshTime;
    }

    // Update is called once per frame
    void Update()
    {
        currentTime += Time.deltaTime;
        if (currentTime >= refreshTime) {
            currentTime = 0;
            generateBlock();
        }
    }

    void generateBlock() {
        if (GameConfig.blockCount >= maxBlockCount) {
            currentTime = refreshTime;
            return;
        }
        var r = random.Next(30, 50);
        var alpha = (float)(random.NextDouble()) * 2 * Mathf.PI;
        var theta = ((float)random.NextDouble() / 4 + 0.5f) * Mathf.PI / 2;
        var scale = (float)random.NextDouble() / 5 + 1;

        var position = new Vector3(r * Mathf.Sin(theta) * Mathf.Cos(alpha), r * Mathf.Sin(theta) * Mathf.Sin(alpha), r * Mathf.Cos(theta));

        GameObject gameObj; 
        if (random.Next() % 2 == 0) {
            gameObj = Instantiate(blockPrefab, position, transform.localRotation) as GameObject;
        } else {
            gameObj = Instantiate(spherePrefab, position, transform.localRotation) as GameObject;
        }
        gameObj.transform.localScale = new Vector3(scale, scale, scale);
        gameObj.GetComponent<Rigidbody>().velocity = new Vector3((float)random.NextDouble() * 4 - 2, (float)random.NextDouble() * 4 - 2, (float)random.NextDouble() * 4 - 2);
        gameObj.transform.rotation = Quaternion.Euler((float)random.NextDouble() * 4 - 2, (float)random.NextDouble() * 4 - 2, (float)random.NextDouble() * 4 - 2);
        PlayableBlockScript block = gameObj.GetComponent<PlayableBlockScript>();
        block.initColor = new Color(1 - (float)random.NextDouble() / 2, 1 - (float)random.NextDouble() / 2, 1 - (float)random.NextDouble() / 2);
        block.initHealth = random.Next(10, 20);

        switch (random.Next(0, 6))
        {
            case 0:
                block.type = BlockType.Trivial;
                break;
            case 1:
                block.type = BlockType.Custom;
                break;
            case 2:
                block.type = BlockType.Outline;
                break;
            case 3:
                block.type = BlockType.Wave;
                break;
            case 4:
                block.type = BlockType.Grass;
                break;
            case 5:
                block.type = BlockType.Hatching;
                break;
            default:
                block.type = BlockType.Trivial;
                break;
        }


        GameConfig.blockCount++;
    }
}
