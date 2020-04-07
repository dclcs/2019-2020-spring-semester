using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class KeyboardController : MonoBehaviour
{
    public Text debugText;
    public GameObject player;
    public Camera cam;
    public GameObject configCanvas;
    public Dropdown blockTypeDropdown;

    private static bool displayDebug;
    private static bool displayMenu;

    public float showTime = 1f;
    private int count = 0;
    private float deltaTime = 0f;
    private float fps;

    // Start is called before the first frame update
    void Start()
    {
        KeyboardController.displayDebug = false;
        KeyboardController.displayMenu = false;
        configCanvas.SetActive(KeyboardController.displayMenu);
        Cursor.visible = KeyboardController.displayMenu;
        blockTypeDropdown.interactable = KeyboardController.displayMenu;
    }

    // Update is called once per frame
    void Update()
    {

        count++;
        deltaTime += Time.deltaTime;
        if (deltaTime >= showTime)
        {
            fps = count / deltaTime;
            float milliSecond = deltaTime * 1000 / count;
            count = 0;
            deltaTime = 0f;
        }

        if (Input.GetKeyUp("f3") || Input.GetKeyUp("v") || Input.GetKeyUp(KeyCode.JoystickButton3))
        {
            KeyboardController.displayDebug = !KeyboardController.displayDebug;
        }

        if (displayDebug)
        {
            debugText.text = string.Format("Position: {0}\nVelocity: {1}\nCam Direction: {2}\nOn Ground: {3}\nFrame per Second: {4}\nVerbose Mode ON\nMinecrappy 0.1", player.transform.position, player.GetComponent<Rigidbody>().velocity, cam.transform.rotation, PlayerController.IsOnGround(), fps);
        }
        else
		{
            debugText.text = "";
		}

        if (Input.GetButtonUp("Options"))
		{
            KeyboardController.displayMenu = !KeyboardController.displayMenu;
            configCanvas.SetActive(KeyboardController.displayMenu);
            Cursor.visible = KeyboardController.displayMenu;
            blockTypeDropdown.interactable = KeyboardController.displayMenu;
        }
    }

    public static bool isPaused()
	{
        return KeyboardController.displayMenu;
	}
}
