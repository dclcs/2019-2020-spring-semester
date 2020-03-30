using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class GameSupervisor : MonoBehaviour
{

    static GameSupervisor instance;

    public GameObject introCanvas;
    public GameObject inGameCanvas;
    /*
     * false - intro page
     * true - in game
     */
    public bool isInGame = false;



    private void Awake()
	{
        if (instance != null)
		{
            Destroy(gameObject);
		}

        instance = this;
	}

    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        if (isInGame)
		{
            introCanvas.SetActive(false);
            inGameCanvas.SetActive(true);
		} else
		{
            introCanvas.SetActive(true);
            inGameCanvas.SetActive(false);
        }
    }
}
