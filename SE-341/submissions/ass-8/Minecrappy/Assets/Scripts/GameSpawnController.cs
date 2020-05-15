using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class GameSpawnController : MonoBehaviour
{
    public GameObject blockPrefab;
    public GameObject spherePrefab;
    public GameObject dingPrefab;
    public GameObject textPrefab;
    public GameObject defaultCamera;

    public static GameSpawnController instance;

    private System.Random random = new System.Random();

    public float refreshTime = 3;
    public int maxBlockCount = 20;
    private float currentTime;

    private int blockBroken = 0;
    private int scoreGain = 0;
    private float timeElapsed = 0;

    public GameObject northText;
    public GameObject westText;
    public GameObject eastText;

    private TMPro.TextMeshPro northTMP;
    private TMPro.TextMeshPro westTMP;
    private TMPro.TextMeshPro eastTMP;

    // Start is called before the first frame update
    void Start()
    {
        currentTime = refreshTime;
        GameSpawnController.instance = this;

        northTMP = northText.GetComponent<TMPro.TextMeshPro>();
        westTMP = westText.GetComponent<TMPro.TextMeshPro>();
        eastTMP = eastText.GetComponent<TMPro.TextMeshPro>();
    }

    // Update is called once per frame
    void Update()
    {
        currentTime += Time.deltaTime;
        if (currentTime >= refreshTime) {
            currentTime = 0;
            generateBlock();
        }

        timeElapsed += Time.deltaTime;

        if (blockBroken == 0) {
            northTMP.text = "no blocks broken";
        } else if (blockBroken == 1) {
            northTMP.text = "1 block broken";
        } else {
            northTMP.text = string.Format("{0} blocks broken", blockBroken);
        }

        westTMP.text = string.Format("score gained: {0}", scoreGain);

        var time = (int)timeElapsed;
        if (time == 0) {
            eastTMP.text = "you started playing just now";
        } else if (time == 1) {
            eastTMP.text = "you've played for 1 second";
        } else {
            eastTMP.text = string.Format("you've played for {0} seconds", time);
        }
    }

    void generateBlock() {
        if (GameConfig.blockCount >= maxBlockCount) {
            currentTime = refreshTime;
            return;
        }
        var r = random.Next(10, 20);
        var alpha = (float)(random.NextDouble()) * 2 * Mathf.PI;
        var theta = ((float)random.NextDouble() / 4 + 0.2f) * Mathf.PI / 2;
        var scale = (float)random.NextDouble() / 5 + 1;

        var position = new Vector3(r * Mathf.Sin(theta) * Mathf.Cos(alpha), r * Mathf.Cos(theta), r * Mathf.Sin(theta) * Mathf.Sin(alpha));

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

    public void addScore(Vector3 position, int score) {
        var textObj = Instantiate(textPrefab, position, Quaternion.Euler(0, defaultCamera.transform.eulerAngles.y, 0)) as GameObject;
        textObj.GetComponent<TMPro.TextMeshPro>().text = string.Format("+{0}", score);
        if (score < 13) {
            textObj.GetComponent<TMPro.TextMeshPro>().color = new Color(0, 1, 1);
        } else if (score < 17) {
            textObj.GetComponent<TMPro.TextMeshPro>().color = new Color(1, 0, 1);
        } else {
            textObj.GetComponent<TMPro.TextMeshPro>().color = new Color(1, 1, 0);
        }
        blockBroken++;
        scoreGain += score;
    }

    public void playDingEffect() {
        Instantiate(dingPrefab);
    }
}
