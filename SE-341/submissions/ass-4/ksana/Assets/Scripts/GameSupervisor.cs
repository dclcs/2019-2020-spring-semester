using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class GameSupervisor : MonoBehaviour
{

    public static GameSupervisor instance;

    public Rigidbody2D player;

    public GameObject introCanvas;
    public GameObject inGameCanvas;
    public GameObject creditsCanvas;

    public Text creditButtonText;
    public Text crystalCountText;

    public GameObject promptIcon;

    /*
     * false - intro page
     * true - in game
     */
    public bool isInGame = false;
    public bool isCreditShown = false;

    public int elementCount = 0;
    public const int maxElementCount = 10;

    public int crystalCount = 0;

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

            if (crystalCount > 1)
            {
                crystalCountText.text = string.Format("You have picked up {0} crystals.", crystalCount);
            }
            else if (crystalCount == 1)
            {
                crystalCountText.text = "You have picked up 1 crystal.";
            }
            else
            {
                crystalCountText.text = "";
            }
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
        isCreditShown = false;
        creditsCanvas.SetActive(false);
        creditButtonText.text = "credits";
        isInGame = true;
        promptIcon.SetActive(true);
    }
}
