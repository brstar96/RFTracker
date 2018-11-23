using System.Collections;
using System.Collections.Generic;
using UnityEngine;
[ExecuteInEditMode]
public class Z_axis_depth_cam_scrp : MonoBehaviour {
    public Material mat;
    public int width = 512;
    public int height = 512;

    private Camera cam;
    private RenderTexture rt;
    private int image_id = 0;
    bool SnapFlag = true;
	// Use this for initialization
	void Start () {
        cam = GetComponent<Camera>();   //获取当前绑定到脚本的相机

        cam.depthTextureMode = DepthTextureMode.Depth;
        
        //		rt = new RenderTexture (width, height, 24);  // 24 bit depth
        //		cam.targetTexture = rt;


    }

    void OnRenderImage(RenderTexture source, RenderTexture destination)
    {
        Graphics.Blit(source, destination, mat);

       
    }
    

    // Update is called once per frame
    void Update()
    {

    }
}
