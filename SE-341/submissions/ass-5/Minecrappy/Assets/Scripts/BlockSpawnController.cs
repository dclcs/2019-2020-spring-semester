using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class BlockSpawnController : MonoBehaviour
{

    public GameObject block;
    public Camera mainCamera;
    public GameObject player;

    public float distance = 2f;
    public float height = 1f;

    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        if (Input.GetMouseButtonUp(1) || Input.GetKeyUp("f"))
		{
            var forwarding = mainCamera.transform.forward;
            Vector3 position = player.transform.position + new Vector3(forwarding.x * distance, height, forwarding.z * distance);

            switch (ConfigController.instance.blockType)
			{
                case BlockType.Trivial:
                    spawnTrivialBlock(position);
                    break;
                case BlockType.Custom:
                    spawnHighlightedBlock(position);
                    break;
			}
        }
    }

    void spawnTrivialBlock(Vector3 position)
	{
        GameObject gameObj = Instantiate(block, position, transform.localRotation) as GameObject;
        BlockShader shader = gameObj.AddComponent(typeof(BlockShader)) as BlockShader;
        shader.generateTrivialBlock(ConfigController.instance.highlightColor);
    }

    void spawnHighlightedBlock(Vector3 position)
    {
        GameObject gameObj = Instantiate(block, position, transform.localRotation) as GameObject;
        BlockShader shader = gameObj.AddComponent(typeof(BlockShader)) as BlockShader;

        if (ConfigController.instance.renderStyle == RenderStyle.Normal)
        {
            shader.generateNormalHighlightBlock(ConfigController.instance.highlightColor, ConfigController.instance.specReflect, ConfigController.instance.renderStyle);
        } else if (ConfigController.instance.renderStyle == RenderStyle.Blinn_Phong)
		{
            shader.generateBlinnPhongHighlightBlock(ConfigController.instance.highlightColor, ConfigController.instance.specReflect, ConfigController.instance.renderStyle);
        }
    }
}
