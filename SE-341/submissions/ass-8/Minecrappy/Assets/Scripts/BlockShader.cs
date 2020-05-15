using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class BlockShader : MonoBehaviour
{
    //public Material[] materials;
    private Renderer renderer;

    public Texture bumpTex;
    public Texture waveTex;
    public Texture noiseTex;
    public Texture whiteTex;
    public Texture blackTex;

    public Texture furry;
    public Texture grass;

    public Material hatchingMaterial;

    // Start is called before the first frame update
    void Start()
    {
        renderer = GetComponent<Renderer>();
        renderer.enabled = true;
    }

    // Update is called once per frame
    void Update()
    {

    }

    public void initRenderer()
	{
        if (renderer == null)
        {
            renderer = GetComponent<Renderer>();
        }
        renderer.enabled = true;
    }

    public void generateTrivialBlock(HighlightColor colorType)
    {
        initRenderer();

        renderer.material = new Material(Shader.Find("Standard"));

        Color color = Utilities.generateColor(colorType);
        MaterialPropertyBlock propertyBlock = new MaterialPropertyBlock();
        renderer.GetPropertyBlock(propertyBlock);
        propertyBlock.SetColor("_Color", color);
        renderer.SetPropertyBlock(propertyBlock);
    }

    public void generateNormalHighlightBlock(HighlightColor colorType, double specReflectStrength, RenderStyle renderStyle)
    {
        initRenderer();

        var material = new Material(Shader.Find("Custom/HighlightShader"));

        Color color = Utilities.generateColor(colorType);

        material.SetColor("_Diffuse", color);
        material.SetColor("_Specular", new Color(1f, 1f, 1f, 1f));
        material.SetInt("_Gloss", 100 - System.Convert.ToInt32(specReflectStrength * 100));

        renderer.material = material;
    }

    public void generateBlinnPhongHighlightBlock(HighlightColor colorType, double specReflectStrength, RenderStyle renderStyle)
    {
        initRenderer();

        var material = new Material(Shader.Find("Custom/BlinnPhongShader"));

        Color color = Utilities.generateColor(colorType);

        material.SetColor("_Diffuse", color);
        material.SetColor("_Specular", new Color(1f, 1f, 1f, 1f));
        material.SetInt("_Gloss", 100 - System.Convert.ToInt32(specReflectStrength * 100));

        renderer.material = material;
    }

    public void generateOutlineBlock(HighlightColor colorType, double strokeWidth) {
        initRenderer();

        var material = new Material(Shader.Find("Custom/OutlineShader"));

        Color color = Utilities.generateColor(colorType);
        material.SetColor("_OutlineColor", color);
        material.SetFloat("_OutlineWidth", System.Convert.ToSingle(strokeWidth / 30 + 0.01));
        material.SetTexture("_MainTex", whiteTex);
        renderer.material = material;
    }

    public void generateWaveBlock(HighlightColor colorType, double waterSpeed) {
        initRenderer();
        var material = new Material(Shader.Find("Custom/WaveShader"));
        // Color color = Utilities.generateColor(colorType);
        // material.SetColor("_SpecColor", color);
        material.SetFloat("_WaterSpeed", 0.1f + System.Convert.ToSingle(waterSpeed));
        material.SetTexture("_WaterTex", bumpTex);
        material.SetTexture("_WaveTex", bumpTex);
        material.SetTexture("_BumpTex", bumpTex);
        material.SetTexture("_GTex", waveTex);
        material.SetTexture("_NoiseTex", noiseTex);
        renderer.material = material;
    }

    public void generateFurryBlock(double furLength) {
        initRenderer();
        var material = new Material(Shader.Find("Doctrina/Fur"));
        material.SetTexture("_MainTex", furry);
        material.SetFloat("_Cutoff", 1f - System.Convert.ToSingle(furLength));
        renderer.material = material;
    }

    public void generateGrassBlock(double grassLength) {
        initRenderer();
        var material = new Material(Shader.Find("Doctrina/Fur"));
        material.SetTexture("_MainTex", grass);
        material.SetFloat("_Cutoff", 1f - System.Convert.ToSingle(grassLength));
        renderer.material = material;
    }

    public void generateHatchingBlock() {
        initRenderer();
        renderer.material = hatchingMaterial;
    }
}
