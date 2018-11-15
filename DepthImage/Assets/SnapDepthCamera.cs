using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.IO;
using System;

public class SnapDepthCamera : MonoBehaviour {
    private Camera cam;
    private RenderTexture rt;
    private GameObject bunny,DepthCamera;
    private int image_id = 0;
    private bool SnapFlag = false;

    public Transform target;
    public Material mat;
    public float MovingSpeed = 0.005f;
    public int width = 512;
    public int height = 512;
    string[] colums;
    byte[] bytes;

    
    public string[] BunnyPositionFileLines,CameraPositionFileLines,CameraPosition;
    StreamWriter writer;

    void Start () {
        /**
         * 뎁스 이미지 받기
         * 
         * **/
        cam = GetComponent<Camera>();   //获取当前绑定到脚本的相机
        cam.depthTextureMode = DepthTextureMode.DepthNormals;
        rt = new RenderTexture(width, height, 32);  // 32 bit depth
        cam.targetTexture = rt;
        writer = new StreamWriter(@"./Assets/datasets.csv");     

        bunny = GameObject.Find("Bunny");
        DepthCamera = GameObject.Find("DepthCamera");

        writer.WriteLine("displacement,Tx,Ty,Tz,Rx,Ry,Rz");

        StartCoroutine("RunMove");

    }

    void OnRenderImage(RenderTexture source, RenderTexture destination)
    {
        Graphics.Blit(source, destination, mat);

        if (SnapFlag)
        {
            RenderTexture currentRT = RenderTexture.active;   
            RenderTexture.active = destination;

            //Texture2D image = new Texture2D(destination.width, destination.height, TextureFormat.RGB24, false);
            Texture2D image = new Texture2D(destination.width, destination.height, TextureFormat.ARGB32, false);
            image.ReadPixels(new Rect(0, 0, destination.width, destination.height), 0, 0);
            image.Apply();
            savePNG(image, @"D:/DepthTestImgs/" + image_id + ".png");
            image_id++;
            RenderTexture.active = currentRT; // restore 
            Destroy(image);
            image = null;
        }
    }


    private void savePNG(Texture2D image, string path_file)
    {

        bytes = image.EncodeToPNG();
       

        System.IO.File.WriteAllBytes(path_file, bytes);


        SnapFlag = false;
    }


    void Update () {
        transform.LookAt(target);
    }

    IEnumerator RunMove()
    {
        CameraPositionFileLines = File.ReadAllLines(@"D:/RFTracker/DepthImage/Assets/CamerePositions.txt");

        for (int CameraPositionLineNO = 0; CameraPositionLineNO < 642; CameraPositionLineNO++)
        {
            CameraPosition = CameraPositionFileLines[CameraPositionLineNO].Split(' ');
            this.transform.position = new Vector3(float.Parse(CameraPosition[0]), float.Parse(CameraPosition[1]), float.Parse(CameraPosition[2]));
            DepthCamera.transform.position = new Vector3(float.Parse(CameraPosition[0]), float.Parse(CameraPosition[1]), float.Parse(CameraPosition[2]));

            // Debug.LogWarning("camera moved ");
            BunnyPositionFileLines = File.ReadAllLines("D:/RFTracker/DepthImage/Assets/RandomizedFiles/RandomizeTest" + (CameraPositionLineNO + 1) + ".csv");
            for (int BunnyPositionLineNo = 0; BunnyPositionLineNo < 2500; BunnyPositionLineNo++)
            {
                colums = BunnyPositionFileLines[BunnyPositionLineNo].Split(',');
                bunny.transform.position = new Vector3(float.Parse(colums[0]) / 2, float.Parse(colums[1]) / 2, float.Parse(colums[2]) / 2);
                writer.WriteLine("displacement," + colums[0] + "," + colums[1] + "," + colums[2] + "," + colums[3] + "," + colums[4] + "," + colums[5] + "," + colums[6]);
                bunny.transform.localEulerAngles = new Vector3(float.Parse(colums[3]), float.Parse(colums[4]), float.Parse(colums[5]));
               // Debug.Log("ImageNo:  " + image_id + "   file Name : RandomizeTest" + (CameraPositionLineNO + 1) + ".csv " + "   lineNo : " + BunnyPositionLineNo + "   translate : " + float.Parse(colums[0]) + "," + colums[1] + "," + colums[2] + "  rotate : " + float.Parse(colums[3]) + "," + colums[4] + "," + colums[5] + "RowNumber : " + colums[6] + "  camarePosition  " + CameraPosition[0] + "  :  " + CameraPosition[1] + "  :  " + CameraPosition[2]);
               
                yield return null;

                SnapFlag = true;
            }
        }
        writer.Close();  
    }

}
