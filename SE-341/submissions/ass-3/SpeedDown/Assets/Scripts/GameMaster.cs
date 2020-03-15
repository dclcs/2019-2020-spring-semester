using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.SceneManagement;

public class GameMaster : MonoBehaviour
{
    public static GameMaster instance;

    public Text timeScore;
    public Text DescriptionLabel;

    public GameObject gameOverPanel;

    private int score;

    private bool gameOver = false;

    // keep it single
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

    public void MakeScoreBonus(int bonus)
	{
        if (!gameOver)
        {
            score += bonus;
        }
	}

    // Update is called once per frame
    void Update()
    {
        if (!gameOver)
        {
            score += 1;
        }
        timeScore.text = string.Format("Score: {0}", score.ToString("00"));
    }

    public static void GameOver(string message)
	{
        instance.gameOverPanel.SetActive(true);
        instance.score = 0;
        instance.DescriptionLabel.text = message;

        // stop the main time tick
        Time.timeScale = 0;

        instance.gameOver = true;
    }

    public void ExitGame()
	{
        // available after packaged and released
        Application.Quit();
	}

    public void RestartGame()
	{
        SceneManager.LoadScene(SceneManager.GetActiveScene().name);
        instance.score = 0;

        // resume the main time tick
        Time.timeScale = 1;

        instance.gameOver = true;
    }
}
