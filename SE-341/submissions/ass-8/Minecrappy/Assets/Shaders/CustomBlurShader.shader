Shader "Custom/CustomBlurShader"
{
    Properties
    {
        _MainTex ("Main Texture", 2D) = "white" {}
        _BlurSize("Blur Size", Range(0, 10)) = 1
    }

    CGINCLUDE
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
    };

    sampler2D _MainTex;
    float2 _MainTex_TexelSize;
    float4 _MainTex_ST;
    
    uniform float _BlurSize;
    uniform float4 _CurWorldPos;
    uniform float4 _LastWorldPos;
    uniform float4x4 _CurVP;
    uniform float4x4 _LastVP;
    uniform sampler2D _BlurTex;             // 运动模糊后的画面 //

    v2f vert (appdata v)
    {
        v2f o;
        o.vertex = UnityObjectToClipPos(v.vertex);
        o.uv = TRANSFORM_TEX(v.uv, _MainTex);
        return o;
    }

    float4 frag1 (v2f i) : SV_Target
    {
        _CurWorldPos.w = 1;
        float4 curClipPos = mul(_CurVP, _CurWorldPos);
        float3 curNDCPos = curClipPos.rgb/curClipPos.w;

        _LastWorldPos.w = 1;
        float4 lastClipPos = mul(_LastVP, _LastWorldPos);
        float3 lastNDCPos = lastClipPos.rgb/lastClipPos.w;

        float2 speed = (curNDCPos.xy - lastNDCPos.xy)*0.5;              // 转到ndc空间做速度计算 //
        float4 finalColor = float4(0,0,0,0);                            // 有颜色的部分透明度大于0, 其余部分透明度等于0 //
        for(int j=0; j<4; j++)
        {
            float2 tempUV = i.uv+j*speed*_BlurSize;
            finalColor += tex2D(_MainTex, tempUV);
        }
        finalColor *= 0.25;
        return finalColor;
    }

    float4 frag2 (v2f i) : SV_Target
    {
        float4 mainTex = tex2D(_MainTex, i.uv);
        float4 blurTex = tex2D(_BlurTex, i.uv);
        float4 finalColor = float4(0,0,0,1);
        if(blurTex.a > 0)
        {
            finalColor.rgb = lerp(mainTex.rgb, blurTex.rgb, blurTex.a);
        }
        else
        {
            finalColor.rgb = mainTex.rgb;
        }
        return finalColor;
    }
    ENDCG

    SubShader
    {
        Tags { "Queue"="Geometry" "RenderType"="Opaque" "IgnoreProjector"="True" }


        Cull Off
        ZWrite Off
        ZTest Always
        
        LOD 100

        // #0 //
        Pass
        {
            CGPROGRAM
            #pragma vertex vert
            #pragma fragment frag1
            ENDCG
        }

        // #1 //
        Pass
        {
            CGPROGRAM
            #pragma vertex vert
            #pragma fragment frag2          
            ENDCG
        }
    }
    
    Fallback Off
}