using UnityEngine;

public class MotionBlurController : MonoBehaviour
{
    [Range(0, 0.5f)]
    public float BlurSize;

    private Material m_mat;
    private const string ShaderName = "Custom/MotionBlurShader";
    private Matrix4x4 m_curVP_Inverse;
    private Matrix4x4 m_lastVP;
    private Camera m_cam;

    void Start()
    {
        Shader shader = Shader.Find(ShaderName);
        if (shader == null)
        {
            enabled = false;
            return;
        }

        m_mat = new Material(shader);

        m_cam = Camera.main;
        if (m_cam == null)
        {
            enabled = false;
            return;
        }

        m_cam.depthTextureMode = DepthTextureMode.Depth;
    }

    void OnRenderImage(RenderTexture srcRT, RenderTexture dstRT)
    {
        if (m_mat == null || m_cam == null)
        {
            return;
        }

        Matrix4x4 curVP = m_cam.projectionMatrix*m_cam.worldToCameraMatrix;
        m_curVP_Inverse = curVP.inverse;

        m_mat.SetFloat("_BlurSize", BlurSize);
        m_mat.SetMatrix("_CurVPInverse", m_curVP_Inverse);
        m_mat.SetMatrix("_LastVP", m_lastVP);

        Graphics.Blit(srcRT, dstRT, m_mat, 0);

        m_lastVP = curVP;
    }
}