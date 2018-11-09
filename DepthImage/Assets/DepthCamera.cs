using System.Collections;
using System.Collections.Generic;
using UnityEngine;
[ExecuteInEditMode]
public class DepthCamera : MonoBehaviour {
    public Material mat;
    public int width = 512;
    public int height = 512;

    private Camera cam;
    private RenderTexture rt;
    private int image_id = 0;
    // Use this for initialization
    void Start () {
        cam = GetComponent<Camera>();   //获取当前绑定到脚本的相机

        cam.depthTextureMode = DepthTextureMode.Depth;

    }
    void OnRenderImage(RenderTexture source, RenderTexture destination)
    {
        Graphics.Blit(source, destination, mat);
        //mat is the material which contains the shader
        //we are passing the destination RenderTexture to
    }
    // Update is called once per frame
    void Update () {
		
	}
}
