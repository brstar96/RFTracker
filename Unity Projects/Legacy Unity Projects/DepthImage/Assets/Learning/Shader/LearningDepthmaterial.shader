Shader "Custom/LearningDepthmaterial" {
	Properties{
		_Color("Color", Color) = (1,1,1,1)
		_MainTex("Albedo (RGB)", 2D) = "white" {}
	_Glossiness("Smoothness", Range(0,1)) = 0.5
		_Metallic("Metallic", Range(0,1)) = 0.0
	}
		SubShader{
		Tags{ "RenderType" = "Opaque" }

		Pass{
		CGPROGRAM
#pragma vertex vert
#pragma fragment frag
#include "UnityCG.cginc"

		sampler2D _CameraDepthTexture;

	struct v2f {
		float4 pos : SV_POSITION;
		float4 scrPos:TEXCOORD1;
	};

	//Vertex Shader
	v2f vert(appdata_base v) {
		v2f o;
		//Transforms a point from object space to the camera’s clip space in homogeneous coordinates.
		o.pos = UnityObjectToClipPos(v.vertex);
		o.scrPos = ComputeScreenPos(o.pos);
		//for some reason, the y position of the depth texture comes out inverted
		//o.scrPos.y = 1 - o.scrPos.y;
		return o;
	}

	//Fragment Shader
	half4 frag(v2f i) : COLOR{
		//Linear01Depth는 0~255로 표현된 값을 0~1로 정규화해주는 함수. 
		//UNITY_SAMPLE_DEPTH는 깊이 정보를 가져오는 내장함수
		//float depthValue = Linear01Depth(tex2Dproj(_CameraDepthTexture, UNITY_PROJ_COORD(i.scrPos)).r);
		float depthValue = 1 - (UNITY_SAMPLE_DEPTH(tex2Dproj(_CameraDepthTexture, UNITY_PROJ_COORD(i.scrPos)).r));
	half4 depth;

	depth.r = depthValue;
	depth.g = depthValue;
	depth.b = depthValue;

	depth.a = 1;
	return depth;
	}
		ENDCG
	}
	}
		FallBack "Diffuse"

}
