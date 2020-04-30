Shader "Custom/MotionBlurShader"
{
	Properties
	{
		_MainTex ("Main Texture", 2D) = "white" {}
		_BlurSize("Blur Size", Range(0, 10)) = 1
	}

	SubShader
	{
		Tags { "Queue"="Transparent" "RenderType"="Transparent" "IgnoreProjector"="True" }

		Cull Off
		ZWrite Off
		ZTest Always
		
		LOD 100

		Pass
		{
			CGPROGRAM
			#pragma vertex vert
			#pragma fragment frag			
			#include "UnityCG.cginc"			

			struct appdata
			{
				float4 vertex : POSITION;
				float2 uv : TEXCOORD0;
			};

			struct v2f
			{
				float4 vertex : SV_POSITION;
				float2 uv : TEXCOORD0;
				float2 uv_depth : TEXCOORD1;
			};

			sampler2D _MainTex;
			float2 _MainTex_TexelSize;
			float4 _MainTex_ST;
			sampler2D _CameraDepthTexture;

			uniform float _BlurSize;
			uniform float4x4 _CurVPInverse;
			uniform float4x4 _LastVP;

			v2f vert (appdata v)
			{
				v2f o;
				o.vertex = UnityObjectToClipPos(v.vertex);
				o.uv = TRANSFORM_TEX(v.uv, _MainTex);
				o.uv_depth = TRANSFORM_TEX(v.uv, _MainTex);

			#if UNITY_UV_STARTS_AT_TOP
				if (_MainTex_TexelSize.y < 0)
				{
					o.uv_depth.y = 1-o.uv_depth.y;
				}
			#endif
				return o;
			}

			float4 frag (v2f i) : SV_Target
			{
				float2 uv = i.uv;
				float depth = tex2D(_CameraDepthTexture, i.uv_depth);

				// float depth = SAMPLE_DEPTH_TEXTURE(_CameraDepthTexture, i.uv_depth);
				// depth = Linear01Depth(depth);

				float4 curNDCPos = float4(uv.x*2-1, uv.y*2-1, depth*2-1, 1);
				float4 worldPos = mul(_CurVPInverse, curNDCPos);
				worldPos /= worldPos.w;											// 为了确保世界空间坐标的w分量为1 //
				// worldPos.w = 1;
				float4 lastClipPos = mul(_LastVP, worldPos);
				float4 lastNDCPos = lastClipPos/lastClipPos.w;					// 一定要除以w分量, 转换到 NDC空间, 然后再做比较 //

				float2 speed = (curNDCPos.xy - lastNDCPos.xy)*0.5;				// 转到ndc空间做速度计算 //
				float4 finalColor = float4(0,0,0,1);
				for(int j=0; j<4; j++)
				{
					float2 tempUV = uv+j*speed*_BlurSize;
					finalColor.rgb += tex2D(_MainTex, tempUV).rgb;
				}
				finalColor *= 0.25;
				return finalColor;				
			}
			ENDCG
		}
	}
	
	Fallback Off
}