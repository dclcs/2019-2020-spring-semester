Shader "Unlit/Dynamic"
{
    Properties
    {
        _MainColor ("Main Color", Color) = (1,1,1,1)
        _MainTex ("Main Texture", 2D) = "white" {}
    }
    SubShader
    {
        Tags { "RenderType"="Opaque" }
        LOD 100

        Pass
        {
            CGPROGRAM
            #pragma vertex vert
            #pragma fragment frag
            // make fog work
            #pragma multi_compile_fog
            #pragma shader_feature USE_TEXTURE
            #pragma shader_feature USE_NORMAL
            #pragma shader_feature USE_TRIVIAL


            sampler2D _MainTex;

            #include "UnityCG.cginc"

            struct VertexData
            {
                float4 position : POSITION;
                float3 normal : NORMAL;
                float2 uv : TEXCOORD0;
            };

            struct FragmentData
            {
                float4 position : SV_POSITION;
                float3 normal : NORMAL;
                float2 uv : TEXCOORD0;
            };

            float4 _MainColor;

            FragmentData vert(VertexData v) {
                FragmentData i;
                i.position = UnityObjectToClipPos(v.position);
                i.normal = UnityObjectToWorldNormal(v.normal);
                i.uv = v.uv;
                return i;
            }

            float4 frag (FragmentData i) : SV_TARGET {
                #if USE_TRIVIAL
                return _MainColor;
                #endif

                #if USE_NORMAL
                return float4(i.normal, 1);
                #endif

                #if USE_TEXTURE
                return tex2D(_MainTex, i.uv);
                #endif

                // should never execute here
                return float4(1,0.5,1,1);
            }
            ENDCG
        }
    }

    CustomEditor "CustomShaderGUI"
}
