using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System;
using System.IO;

public class WorkingScr : MonoBehaviour
{

    public Material mat;
    public int width = 512;
    public int height = 512;

    private Camera cam;
    private RenderTexture rt;
    private int image_id = 0;
    private bool SnapFlag = false;

    void Start()
    {
        cam = GetComponent<Camera>();   //获取当前绑定到脚本的相机

        cam.depthTextureMode = DepthTextureMode.Depth;

        rt = new RenderTexture(width, height, 24);  // 24 bit depth
        cam.targetTexture = rt;
        SnapFlag = true;

        createFrontFace("D:/DepthTestImgs/img.png");

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

            savePNG(image, "D:/DepthTestImgs/img.png");

            image_id++;
            RenderTexture.active = currentRT; // restore 
        }
    }

    private void savePNG(Texture2D image, string path_file)
    {
        byte[] bytes = image.EncodeToPNG();
        System.IO.File.WriteAllBytes(path_file, bytes);
        SnapFlag = false;
    }

    public void createFrontFace(String path)
    {
        List<Vector3> grayScaleList = loadGrayScalePNG(path);
        List<GameObject> SphereList = new List<GameObject>(); //게임씬에 Sphere를 생성하기 위한 리스트
        List<Vector3> vertexPositions = new List<Vector3>();
        GameObject singleSphere;
        for (int j = 0; j < grayScaleList.Count; j++)
        {
            singleSphere =(GameObject.CreatePrimitive(PrimitiveType.Sphere));
            singleSphere.transform.localScale = new Vector3(0.0009f, 0.0009f, 0.0009f);
            SphereList.Add(singleSphere);
        }

        for (int count = 0; count < grayScaleList.Count; count++)
        {
            vertexPositions.Add(new Vector3(grayScaleList[count].x / 512f, grayScaleList[count].y / 512f, grayScaleList[count].z / 2.55f-0.5f));
        }

        for (int j = 0; j < SphereList.Count; j++)
        {
            //SphereList[j].transform.position = new Vector3(vertexPositions[j].x, vertexPositions[j].y, vertexPositions[j].z);
            SphereList[j].transform.position = new Vector3(vertexPositions[j].x - 0.5f, vertexPositions[j].y - 0.5f, vertexPositions[j].z * 2.5f+0.6f);
            // getDisplacement(); //call 'getDisplacement' function for calculate displacement value per 'GT-backprojected vertex pair'. (11.18)
        }
    }


    
    public List<Vector3> loadGrayScalePNG(string filePath)
    {
        List<Vector3> grayScalePixcels = new List<Vector3>();
        Color pixcels;
        Texture2D tex = null;
        byte[] fileData;

        if (File.Exists(filePath))
        {
            fileData = File.ReadAllBytes(filePath);
            tex = new Texture2D(512, 512);
            tex.LoadImage(fileData); //..this will auto-resize the texture dimensions.
        }

        for (int height = 0; height < 512; height++)
        {
            for (int width = 0; width < 512; width++)
            {
                pixcels = tex.GetPixel(height, width);
                if (pixcels.r < 1)
                {
                    grayScalePixcels.Add(new Vector3(height, width, pixcels.r));
                }
            }
        }

        return grayScalePixcels;

        Destroy(tex);
        grayScalePixcels = null;
        fileData = null;
    }

}
