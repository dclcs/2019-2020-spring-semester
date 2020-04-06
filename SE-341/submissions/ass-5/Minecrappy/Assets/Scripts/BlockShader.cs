using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class BlockShader : MonoBehaviour
{
    //public Material[] materials;
    private Renderer renderer;

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
}
