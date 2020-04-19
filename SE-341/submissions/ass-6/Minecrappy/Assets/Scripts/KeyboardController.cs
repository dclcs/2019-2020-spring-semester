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

    public GameObject DoFEffectManager;

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
            debugText.text = string.Format("Position: {0}\nVelocity: {1}\nCam Direction: {2}\nOn Ground: {3}\n\nResolution: ({5}, {6})\nFrame per Second: {4}\n\nVerbose Mode: True\nFoggy Effect: {7}\nDepth of Field Effect: {8}\nMotion Blur Effect: {9}\n\nMinecrappy 0.2", player.transform.position, player.GetComponent<Rigidbody>().velocity, cam.transform.rotation, PlayerController.IsOnGround(), fps, UnityEngine.Screen.width, UnityEngine.Screen.height, ConfigController.instance.foggyEffect, ConfigController.instance.depthOfFieldEffect, ConfigController.instance.motionBlurEffect);
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

        if (Input.GetButtonUp("TriggerFoggyEffect")) {
            ConfigController.instance.foggyEffect = !ConfigController.instance.foggyEffect;
            cam.GetComponent<CameraFoggyController>().enabled = ConfigController.instance.foggyEffect;
        }

        if (Input.GetButtonUp("TriggerDepthOfField")) {
            ConfigController.instance.depthOfFieldEffect = !ConfigController.instance.depthOfFieldEffect;
            DoFEffectManager.SetActive(ConfigController.instance.depthOfFieldEffect);
        }

        if (Input.GetButtonUp("TriggerMotionBlurEffect")) {
            ConfigController.instance.motionBlurEffect = !ConfigController.instance.motionBlurEffect;
            cam.GetComponent<MotionBlurController>().enabled = ConfigController.instance.motionBlurEffect;
        }
    }

    public static bool isPaused()
	{
        return KeyboardController.displayMenu;
	}
}
