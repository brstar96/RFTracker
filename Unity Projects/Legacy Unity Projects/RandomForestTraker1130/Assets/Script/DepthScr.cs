﻿using System.Collections;
using System.Collections.Generic;
using UnityEngine;

//게임플레이 뷰에 Depth Image를 출력하기 위한 스크립트
[ExecuteInEditMode]
public class DepthScr : MonoBehaviour {

    public Material mat;
    public int width = 512;
    public int height = 512;

    private Camera cam;
    //private RenderTexture rt;
    private int image_id = 0;

    void Start()
    {
        cam = GetComponent<Camera>();   //获取当前绑定到脚本的相机

        cam.depthTextureMode = DepthTextureMode.Depth;

        //		rt = new RenderTexture (width, height, 24);  // 24 bit depth
        //		cam.targetTexture = rt;
    }

    void OnRenderImage(RenderTexture source, RenderTexture destination)
    {
        Graphics.Blit(source, destination, mat);
        //mat is the material which contains the shader
        //we are passing the destination RenderTexture to
    }

    // Update is called once per frame
    void Update()
    {

    }
    
}
