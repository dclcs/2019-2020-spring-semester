﻿using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlayableBlockScript : MonoBehaviour
{

    public Texture bumpTex;
    public Texture waveTex;
    public Texture noiseTex;
    public Texture whiteTex;
    public Texture blackTex;
    public Texture grass;

    public Material hatchingMaterial;

    public double specReflectStrength = 0.5;
    public double grassLength = 0.5;
    public double waterSpeed = 0.5;
    public double strokeWidth = 0.5;

    public Color initColor;
    public float initHealth;

    private Color currentColor;
    private float currentHealth;
    
    public float healthDestroySpeed = 1;

    private bool isGazeEntering = false;

    public BlockType type = BlockType.Trivial;

    private Rigidbody rb;

    // Start is called before the first frame update
    void Start()
    {
        currentColor = initColor;
        currentHealth = initHealth;
        refreshMaterial();

        rb = GetComponent<Rigidbody>();
    }

    // Update is called once per frame
    void Update()
    {
        if (isGazeEntering) {
            currentHealth -= Time.deltaTime * healthDestroySpeed;
            if (currentHealth <= 0) {
                // 加分
                GameSpawnController.instance.playDingEffect();
                GameSpawnController.instance.addScore(transform.position, (int)initHealth);
                GameConfig.blockCount--;
                Destroy(gameObject);
            }
        } else {
            if (currentHealth <= 0) {
                // 不加分
                GameConfig.blockCount--;
                Destroy(gameObject);
            }
        }
        if (gameObject.transform.position.y < 0) {
            currentHealth -= Time.deltaTime * healthDestroySpeed;
        }
        
        currentColor = new Color(initColor.r * currentHealth / initHealth, initColor.g * currentHealth / initHealth, initColor.b * currentHealth / initHealth, currentHealth / initHealth);
        refreshMaterial();
    }

    public void refreshMaterial() {
        switch (type)
        {
            case BlockType.Trivial:
                generateTrivialBlock();
                break;
            case BlockType.Custom:
                generateNormalHighlightBlock();
                break;
            case BlockType.Outline:
                generateOutlineBlock();
                break;
            case BlockType.Wave:
                generateWaveBlock();
                break;
            case BlockType.Furry:
                generateFurryBlock();
                break;
            case BlockType.Grass:
                generateGrassBlock();
                break;
            case BlockType.Hatching:
                generateHatchingBlock();
                break;
            default:
                Destroy(gameObject);
                GameConfig.blockCount--;
                return;
        }
    }


    public void onGazeEnter() {
        isGazeEntering = true;
    }

    public void onGazeLeave() {
        isGazeEntering = false;
    }

    public void initRenderer()
	{
        GetComponent<Renderer>().enabled = true;
    }

    public void generateTrivialBlock()
    {
        GetComponent<Renderer>().material = new Material(Shader.Find("Standard"));

        MaterialPropertyBlock propertyBlock = new MaterialPropertyBlock();
        GetComponent<Renderer>().GetPropertyBlock(propertyBlock);
        propertyBlock.SetColor("_Color", currentColor);
        GetComponent<Renderer>().SetPropertyBlock(propertyBlock);
    }

    public void generateNormalHighlightBlock()
    {
        var material = new Material(Shader.Find("Custom/HighlightShader"));

        material.SetColor("_Diffuse", currentColor);
        material.SetColor("_Specular", new Color(1f, 1f, 1f, 1f));
        material.SetInt("_Gloss", 100 - System.Convert.ToInt32(specReflectStrength * 100));

        GetComponent<Renderer>().material = material;
    }

    public void generateOutlineBlock() {
        var material = new Material(Shader.Find("Custom/OutlineShader"));

        material.SetColor("_OutlineColor", currentColor);
        material.SetFloat("_OutlineWidth", System.Convert.ToSingle(strokeWidth / 30 + 0.01));
        material.SetTexture("_MainTex", whiteTex);
        GetComponent<Renderer>().material = material;
    }

    public void generateWaveBlock() {
        var material = new Material(Shader.Find("Custom/WaveShader"));
        material.SetColor("_SpecColor", currentColor);
        material.SetFloat("_WaterSpeed", 0.1f + System.Convert.ToSingle(waterSpeed));
        material.SetTexture("_WaterTex", bumpTex);
        material.SetTexture("_WaveTex", bumpTex);
        material.SetTexture("_BumpTex", bumpTex);
        material.SetTexture("_GTex", waveTex);
        material.SetTexture("_NoiseTex", noiseTex);
        GetComponent<Renderer>().material = material;
    }

    public void generateFurryBlock() {

    }

    public void generateGrassBlock() {
        var material = new Material(Shader.Find("Doctrina/Fur"));
        material.SetTexture("_MainTex", grass);
        material.SetFloat("_Cutoff", 1f - System.Convert.ToSingle(grassLength));
        GetComponent<Renderer>().material = material;
    }

    public void generateHatchingBlock() {
        GetComponent<Renderer>().material = hatchingMaterial;
    }
}