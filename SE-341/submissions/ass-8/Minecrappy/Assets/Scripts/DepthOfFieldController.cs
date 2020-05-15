using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class DepthOfFieldController : MonoBehaviour
{
    #region  come from ImageEffectBase.cs
    /// Provides a shader property that is set in the inspector
    /// and a material instantiated from the shader
    public Shader   shader;
    private Material m_Material;

    void OnEnable() {
        GetComponent<Camera>().depthTextureMode |= DepthTextureMode.Depth;        
    }
    
    protected virtual void Start ()
    {

    }
    
    protected Material material {
        get {
            if (m_Material == null) {
                m_Material = new Material (shader);
                //m_Material.hideFlags = HideFlags.HideAndDontSave;
                m_Material.hideFlags = HideFlags.DontSave;
            }
            return m_Material;
        } 
    }
    
    protected virtual void OnDisable() {
        if( m_Material ) {
            //DestroyImmediate( m_Material );
            Destroy( m_Material );
        }
    }
    #endregion
    void OnRenderImage (RenderTexture source, RenderTexture destination) {
        //material.SetTexture ("_MainTex",source);
        material.SetFloat("focalDistance01", 0.0f);
        material.SetFloat("_OffsetDistance", 0.01f);
        Graphics.Blit (source, destination, material);
    }
}