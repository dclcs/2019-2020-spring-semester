using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class GameSupervisor : MonoBehaviour
{

    static GameSupervisor instance;

    public GameObject introCanvas;
    public GameObject inGameCanvas;
    public GameObject creditsCanvas;

    public Text creditButtonText;

    /*
     * false - intro page
     * true - in game
     */
    public bool isInGame = false;
    public bool isCreditShown = false;

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

    public void showCredits()
    {
        if (isCreditShown)
		{
            isCreditShown = false;
            creditsCanvas.SetActive(false);
            creditButtonText.text = "credits";
        } else
		{
            isCreditShown = true;
            creditsCanvas.SetActive(true);
            creditButtonText.text = "hide credits";
        }
    }

    public void exitGame()
	{
        Application.Quit();
	}

    public void startGame()
	{
        isInGame = true;
	}
}
