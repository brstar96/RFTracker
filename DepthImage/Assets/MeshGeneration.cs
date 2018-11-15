using UnityEngine;
using System.Collections.Generic;
using System.IO;

[RequireComponent(typeof(MeshFilter))]
[RequireComponent(typeof(MeshRenderer))]


public class MeshGeneration : MonoBehaviour
{
    string PATH = "D:/DepthTestImgs/328.png";

    void Start()
    {
        CreateCube();
    }

    private void CreateCube()
    {
        Vector3[] vertices = new Vector3[20];

        for (int i = 0; i < 20; i++)
        {
            vertices[i] = new Vector3(0, 0, 0);
        }

        Texture2D GrayscaleImg = new Texture2D(512, 512, TextureFormat.RGBA32, false);
        //string PATH = "DepthSampleImgs/31.png";
        //GrayscaleImg = Resources.Load(PATH, typeof(Texture2D)) as Texture2D;
        //byte[] bytes = new byte[512 * 512];
        //GrayscaleImg.LoadImage(bytes);

        //for (int i = 0; i < 512; i++)
        //{
        //    Debug.Log(bytes[i]);
        //}
        byte[] bytes = new byte[512 * 512];
        Texture2D testtex = LoadPNG(PATH);
       
        testtex.LoadImage(bytes);

        for (int i=0;i<bytes.Length;i++)
        {
            if (bytes[i]<=0)
            {
                Debug.Log(bytes[i]/255);
            }
            //if (i % 100 == 0)
            //{
            //    Debug.Log("i="+i+"pixel value "+bytes[i] / 255);

            //}

        }
        
        
    }

    public Texture2D LoadPNG(string filePath)
    {
        Texture2D tex = null;
        byte[] fileData;
        int[] Test ={
            0,1,2
        };
        if (File.Exists(filePath))
        {
            fileData = File.ReadAllBytes(filePath);
            Debug.Log(fileData[513]);
            tex = new Texture2D(512, 512);
            tex.LoadImage(fileData); //..this will auto-resize the texture dimensions.
        }
        return tex;
        //Destroy(tex);
    }
}


