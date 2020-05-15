using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;



public class ConfigController : MonoBehaviour
{

    public Dropdown blockCategoryDropdown;
    public Dropdown highlightColorDropdown;
    public Dropdown renderStyleDropdown;
    public Slider specReflectStrengthSlider;

    public double specReflect;
    public BlockType blockType;
    public RenderStyle renderStyle;
    public HighlightColor highlightColor;

    public static ConfigController instance;

    public bool depthOfFieldEffect;
    public bool foggyEffect;
    public bool motionBlurEffect;

    // Start is called before the first frame update
    void Start()
    {
        if (ConfigController.instance != null)
		{
            Destroy(gameObject);
		}
        ConfigController.instance = this;
        onBlockCategoryChanged(0);
        onRenderStyleChanged(0);
        onHighlightColorChanged(0);
        onSpecReflectChanged(0.5f);

        depthOfFieldEffect = true;
        foggyEffect = false;
        motionBlurEffect = true;
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    public void onBlockCategoryChanged(int type)
	{
        blockType = (BlockType)System.Enum.ToObject(typeof(BlockType), type);

        if (blockType == BlockType.Trivial)
		{
            highlightColorDropdown.interactable = true;
            renderStyleDropdown.interactable = false;
            specReflectStrengthSlider.interactable = false;
        } else if (blockType == BlockType.Custom)
		{
            highlightColorDropdown.interactable = true;
            renderStyleDropdown.interactable = true;
            specReflectStrengthSlider.interactable = true;
        } else if (blockType == BlockType.Outline) {
            highlightColorDropdown.interactable = true;
            renderStyleDropdown.interactable = false;
            specReflectStrengthSlider.interactable = true;
        } else if (blockType == BlockType.Wave) {
            highlightColorDropdown.interactable = false;
            renderStyleDropdown.interactable = false;
            specReflectStrengthSlider.interactable = true;
        } else if (blockType == BlockType.Furry) {
            highlightColorDropdown.interactable = true;
            renderStyleDropdown.interactable = false;
            specReflectStrengthSlider.interactable = true;
        }
        else if (blockType == BlockType.Grass)
		{
            highlightColorDropdown.interactable = false;
            renderStyleDropdown.interactable = false;
            specReflectStrengthSlider.interactable = true;
        } else if (blockType == BlockType.Hatching) {
            highlightColorDropdown.interactable = false;
            renderStyleDropdown.interactable = false;
            specReflectStrengthSlider.interactable = false;
        }
	}

    public void onRenderStyleChanged(int type)
	{
        renderStyle = (RenderStyle)System.Enum.ToObject(typeof(RenderStyle), type);
	}

    public void onHighlightColorChanged(int type)
	{
        highlightColor = (HighlightColor)System.Enum.ToObject(typeof(HighlightColor), type);
	}

    public void onSpecReflectChanged(float val)
	{
        specReflect = val;
	}
}
