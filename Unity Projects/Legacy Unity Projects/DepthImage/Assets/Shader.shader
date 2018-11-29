Shader "Custom/Shader" {
    Properties {
        _Color ("Color", Color) = (1,1,1,1)
		_MainTex ("Albedo (RGB)", 2D) = "white" {}
		_Glossiness ("Smoothness", Range(0,1)) = 0.5
		_Metallic ("Metallic", Range(0,1)) = 0.0
	}
    /*SubShader 내부에 보면,   sampler2D _MainTex; 부분이 있습니다. 
        이는 프로퍼티에서 제작한 _MainTex 를 사용하기 위해, sampler2D 로 정의해서 
        메모리에 불러오는 부분 입니다.  파일이 있으면 일단 읽어와야 쓸 수 있는것과 같습니다.
        */
    SubShader{
        Tags{ "RenderType" = "Opaque" }
        /*기본적인 pass 명령은 렌더링 설정 명령의 옵션 목록을 가지며,
         선택적으로 사용할 텍스처를 지정합니다.*/
        Pass{
        /*Shader programs은 Cg/HLSL 언어로 작성되어 있습니다.
        Cg 프로그램 스니펫은 CGPROGRAM과 ENDCG 사이에 작성됩니다.
        */
            CGPROGRAM
            /* #pragma 명령문으로 시작합니다. 이 명령은 컴파일하기 
            위해 어떤 쉐이더 함수인지를 가리킵니다.*/
            /*#pragma vertex name - 정점 쉐이더로 정의되는 name 이라는 이름의 함수*/
            #pragma vertex vert
            /*#pragma fragment name - Fragment 쉐이더로 정의되는 name 이라는 이름의 함수*/
            #pragma fragment frag
            #include "UnityCG.cginc"
        /*sampler2D, sampler3D, samplerCUBE HLSL
         키워드를 사용하여 텍스처와 샘플러를 선언합니다.*/   
        sampler2D _CameraDepthTexture;

        struct v2f {
            float4 pos : SV_POSITION;
            
            /*TEXCOORD0 is the first UV coordinate, typically float2, float3 or float4.*/
            float4 scrPos:TEXCOORD1;
        };
        
        /*appdata_base : 정점은 위치, 법선 및 한 개의 텍스처 좌표로 구성됩니다.
        appdata_tan : 정점 위치, 접선, 법선 및 한 개의 텍스처 좌표로 구성됩니다.
        appdata_full: 정점은 위치, 접선, 법선, 4개의 텍스처 좌표와 색상으로 구성됩니다.*/
        //Vertex Shader
        v2f vert(appdata_base v) {
        
            v2f o;
            /*Transforms a point from object space to the camera’s clip
             space in homogeneous coordinates. This is the equivalent of 
             mul(UNITY_MATRIX_MVP, float4(pos, 1.0)), and should be used
             in its place.*/
            o.pos = UnityObjectToClipPos(v.vertex);
            
            
            o.scrPos = ComputeScreenPos(o.pos);
            //for some reason, the y position of the depth texture comes out inverted
            //o.scrPos.y = 1 - o.scrPos.y;
            return o;
        }

        //Fragment Shader
        
        //http://mgun.tistory.com/1861 참고
        half4 frag(v2f i) : COLOR{
            float depthValue = Linear01Depth(tex2Dproj(_CameraDepthTexture, UNITY_PROJ_COORD(i.scrPos)).r);
            half4 depth;
            depth.r = depthValue;
            depth.g = depthValue;
            depth.b = depthValue;
            depth.a = 1;
            depth = depth * 1.1;
            return depth;
            }
            ENDCG
        }
    }
    /* Fallback 의 경우에, SubShader 가 실패할 경우,출력하게될 쉐이더 입니다.
    Diffuse 를 최종적으로 출력하게 되어있습니다.Mobile 카테고리에 가 보면 Diffuse 쉐이더가 
    있으니 참고하시기 바랍니다.*/
}
