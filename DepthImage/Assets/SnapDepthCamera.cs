using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.IO;
using System;
using System.Threading;

//[ExecuteInEditMode]
public class SnapDepthCamera : MonoBehaviour {
    public Material mat;
    public int width = 512;
    public int height = 512;
    private Camera cam;
    private RenderTexture rt;

    private int image_id = 0;
    private bool SnapFlag = false;
    string[] files;
    string[] lines;
    GameObject bunny;

    // Use this for initialization
    void Start () {
        cam = GetComponent<Camera>();   //获取当前绑定到脚本的相机
        cam.depthTextureMode = DepthTextureMode.Depth;
        rt = new RenderTexture(width, height, 24);  // 24 bit depth
        cam.targetTexture = rt;

        string path = @"D:\RFTracker\DepthImage\Assets\RandomizedFiles";
        bunny = GameObject.Find("StanfordBunny");
        files = Directory.GetFiles(path, "*.csv");

        Vector3 vec = new Vector3(0,0,0);
        bunny.transform.Translate(vec);

        bunny.transform.Rotate(0, 0, 0);

        for (int fileNo=0; fileNo < 1; fileNo++)
        {
            lines = File.ReadAllLines(files[fileNo]);
            for (int lineNo = 0; lineNo < lines.Length; lineNo++)
            {
                string[]colums = lines[lineNo].Split(',');
                
                bunny.transform.position = new Vector3(float.Parse(colums[0]), float.Parse(colums[1]), float.Parse(colums[2]));

                bunny.transform.localEulerAngles = new Vector3(float.Parse(colums[3]), float.Parse(colums[4]), float.Parse(colums[5]));
                Debug.Log("fileNo"+fileNo+"lineNo:"+lineNo+"translate:"+ float.Parse(colums[0]) + ","+colums[1] + ","+colums[2] + "loate:" + float.Parse(colums[3])+","+ colums[4]+","+colums[5]);

                }
        }






        
        //SnapFlag = true;
    }





    void OnRenderImage(RenderTexture source, RenderTexture destination)
    {
        Graphics.Blit(source, destination, mat);
        //mat is the material which contains the shader
        //we are passing the destination RenderTexture to

        if (SnapFlag)
        {
            RenderTexture currentRT = RenderTexture.active;   // save current active rendertexture
            RenderTexture.active = destination;

            Texture2D image = new Texture2D(destination.width, destination.height, TextureFormat.RGB24, false);
            image.ReadPixels(new Rect(0, 0, destination.width, destination.height), 0, 0);
            image.Apply();

            savePNG(image, "D:/RFTracker/DepthImage/Assets/DepthImagesStore/" + image_id + ".png");

            image_id++;
            RenderTexture.active = currentRT; // restore 
        }
    }


    private void savePNG(Texture2D image, string path_file)
    {
        // store the texture into a .PNG file
        byte[] bytes = image.EncodeToPNG();

        // save the encoded image to a file
        System.IO.File.WriteAllBytes(path_file, bytes);
        SnapFlag = false;
    }
    
	
	// Update is called once per frame
	void Update () {

        //    if (Input.GetKeyDown("p"))
        //    {
        //        SnapFlag = true;
        //        Debug.Log("save ");
        //    }
        //    else
        //        SnapFlag = false;

        //
    }
}
