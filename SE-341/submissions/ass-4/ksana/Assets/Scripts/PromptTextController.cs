using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class PromptTextController : MonoBehaviour
{

    public Text promptText;
    public Text crystalText;

    private List<string> promptStrings = new List<string>();
    private List<string> shuffledString = new List<string>();

    public float textTime = 8f;
    public float transitionTime = 0.5f;

    // Start is called before the first frame update
    void Start()
    {
        promptStrings.Add("hint: press W, S or A, D to accelerate vertically or horizontally");
        promptStrings.Add("hint: press Z to end this game");
        promptStrings.Add("hint: approaching red stars attracts you");
        promptStrings.Add("hint: approaching grey stars repulses you");
        promptStrings.Add("collect some crystals if you wish");
        promptStrings.Add("this game will never be over...");
        promptStrings.Add("licensed under GPL-v3");
        promptStrings.Add("to be, or not to be...");
        promptStrings.Add("the answer to life the universe and everything");

        var r = new System.Random();

        while (promptStrings.Count > 0)
        {
            var i = r.Next(promptStrings.Count);
            shuffledString.Add(promptStrings[i]);
            promptStrings.RemoveAt(i);
        }
    }

    // Update is called once per frame
    void Update()
    {
        var currentTime = Time.timeSinceLevelLoad;
        var currentStage = System.Convert.ToInt32(System.Math.Floor(currentTime / textTime)) % shuffledString.Count;
        var internalTime = currentTime - System.Math.Floor(currentTime / textTime) * textTime;

        //Debug.Log(string.Format("currentTime: {0}  currentStage: {1}  internalTime: {2}", currentTime, currentStage, internalTime));
        promptText.text = shuffledString[currentStage];

        Color color = promptText.color;

        if (internalTime < transitionTime)
		{
            float alpha = System.Convert.ToSingle(internalTime / transitionTime);
            color.a = alpha;
		} else if (internalTime > textTime - transitionTime)
		{
            float alpha = System.Convert.ToSingle((textTime - internalTime) / transitionTime);
            color.a = alpha;
        } else
		{
            float alpha = 1.0f;
            color.a = alpha;
		}

        promptText.color = color;
    }
}