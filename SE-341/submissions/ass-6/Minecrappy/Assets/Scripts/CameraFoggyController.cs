using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CameraFoggyController: ScreenEffectBase {
	private const string _FrustumCornersRay = "_FrustumCornersRay";

	private const string _FogDensity = "_FogDensity";
	private const string _FogColor = "_FogColor";
	private const string _FogUnderStart = "_FogUnderStart";
	private const string _FogTopEnd = "_FogTopEnd";

	private Camera myCamera;
	public Camera MyCamera {
		get {
			if (myCamera == null) myCamera = GetComponent < Camera > ();
			return myCamera;
		}
	}

	private Transform myCameraTran;
	public Transform MyCameraTran {
		get {
			if (myCameraTran == null) myCameraTran = MyCamera.transform;
			return myCameraTran;
		}
	}

	[Range(0, 3)]
	public float fogDensity = 1.0f; //控制雾的浓度
	public Color fogColor = Color.white;
	public float fogUnderStart = 0.0f; //雾起始高度
	public float fogTopEnd = 2.0f; //雾结束高度

	private void OnEnable() {
		MyCamera.depthTextureMode |= DepthTextureMode.Depth;
	}

	private void OnDisable() {
		MyCamera.depthTextureMode &= ~DepthTextureMode.Depth;
	}

	private void OnRenderImage(RenderTexture source, RenderTexture destination) {
		if (Material != null) {
			//需要传递的四个角相对于摄像机的方向向量，这里用矩阵的每一行来表示
			Matrix4x4 frustumCorners = Matrix4x4.identity;

			float fov = MyCamera.fieldOfView;
			float near = MyCamera.nearClipPlane;
			float aspect = MyCamera.aspect;

			//计算近裁剪平面三个标准方向
			float halfHeight = near * Mathf.Tan(fov * .5f * Mathf.Deg2Rad);
			Vector3 toTop = halfHeight * MyCameraTran.up;
			Vector3 toRight = halfHeight * MyCameraTran.right * aspect;
			Vector3 toForward = near * MyCameraTran.forward;

			//用三个标准方向重构四个顶点关于摄像机的向量
			Vector3 topRight = toForward + toRight + toTop;
			topRight /= near;

			Vector3 topLeft = toForward - toRight + toTop;
			topLeft /= near;

			Vector3 bottomRight = toForward + toRight - toTop;
			bottomRight /= near;

			Vector3 bottomLeft = toForward - toRight - toTop;
			bottomLeft /= near;

			//用矩阵的每一行来存储这些向量，这里的顺利要与之后解析的顺序对应
			frustumCorners.SetRow(0, topLeft);
			frustumCorners.SetRow(1, topRight);
			frustumCorners.SetRow(2, bottomLeft);
			frustumCorners.SetRow(3, bottomRight);

			//传递向量矩阵和对应的参数
			Material.SetMatrix(_FrustumCornersRay, frustumCorners);

			Material.SetFloat(_FogDensity, fogDensity);
			Material.SetColor(_FogColor, fogColor);
			Material.SetFloat(_FogUnderStart, fogUnderStart);
			Material.SetFloat(_FogTopEnd, fogTopEnd);

			Graphics.Blit(source, destination, Material);
		}
		else Graphics.Blit(source, destination);
	}
}