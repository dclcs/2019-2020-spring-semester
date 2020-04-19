Shader "Custom/FoggyShader"
 {
     Properties
    {
        _MainTex ("Texture", 2D) = "white" {}
    }
    SubShader
     {
        Pass
         {
             CGPROGRAM
             #pragma vertex vert
             #pragma fragment frag
 
             #include "UnityCG.cginc"
 
             //对应四个顶点的射线矩阵
             float4x4 _FrustumCornersRay;
 
             sampler2D _MainTex;
             half4 _MainTex_TexelSize;
             sampler2D _CameraDepthTexture;
             half _FogDensity;
             fixed4 _FogColor;
             float _FogUnderStart;
             float _FogTopEnd;
 
             struct appdata
             {
                 float4 vertex : POSITION;
                 float2 uv : TEXCOORD0;
             };
 
             struct v2f
             {
                 half4 uv : TEXCOORD0;
                 float4 vertex : SV_POSITION;
                 //顶点着色器输出的插值射线
                 float4 interpolatedRay:TEXCOORD1;
             };
 
             v2f vert (appdata v)
             {
                 v2f o;
                 o.vertex = UnityObjectToClipPos(v.vertex);
                 o.uv.xy = v.uv;
                 o.uv.zw=v.uv;//zw存深度纹理
 
                 //对插值射线的索引进行解析，判定该顶点是四个角中的哪一个
                 int idx=0;
                 if(v.uv.x>.5f&&v.uv.y>.5f)
                     idx=1;
                 else if(v.uv.x<.5f&&v.uv.y<.5f)
                     idx=2;
                 else if(v.uv.x>.5f&&v.uv.y<.5f)
                     idx=3;
 
                 //主纹理外的纹理要进行平台差异化处理，同时对顶点的索引也需要进行处理（左上对左下，右上对右下）
                 #if UNITY_UV_STARTS_AT_TOP
                 if(_MainTex_TexelSize.y<0){
                     o.uv.w=1-o.uv.w;
                     idx=idx<2?idx+2:idx-2;
                 }                    
                 #endif
                 
                 //按照解析的索引值得到需要传递的插值射线
                 o.interpolatedRay=_FrustumCornersRay[idx];
 
                 return o;
             }
 
             fixed4 frag (v2f i) : SV_Target
             {
                 //计算像素在世界空间中的位置
                 float linearDepth=LinearEyeDepth(SAMPLE_DEPTH_TEXTURE(_CameraDepthTexture,i.uv.zw));
                 float3 worldPos=_WorldSpaceCameraPos+linearDepth*i.interpolatedRay.xyz;
 
                 //计算雾效系数，这里主要用的关于世界空间高度的线性雾计算
                 float fogDensity=(_FogTopEnd-worldPos.y)/(_FogTopEnd-_FogUnderStart);
                 fogDensity=saturate(fogDensity*_FogDensity);
 
                 //插值得到最终雾效
                 fixed4 col = tex2D(_MainTex, i.uv);
                 col.rgb=lerp(col.rgb,_FogColor.rgb,fogDensity);
 
                 return col;
             }
             ENDCG
         }
     }
}