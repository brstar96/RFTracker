using UnityEngine;
using System.Collections.Generic;
using System.IO;

[RequireComponent(typeof(MeshFilter))]
[RequireComponent(typeof(MeshRenderer))]


public class MeshGeneration : MonoBehaviour
{
    string PATH = "D:/DepthTestImgs/0.png";

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
        byte[] bytes = new byte[512 * 512];
        Texture2D testtex = LoadPNG(PATH);
        Color[] pixcels = testtex.GetPixels(0,0,512,512);
        Debug.Log("length: "+pixcels.Length);
        for(int i=0;i<pixcels.Length;i++)
        {
            if(pixcels[i].r > 0)
            {

            }
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
            tex = new Texture2D(512, 512);
            tex.LoadImage(fileData); //..this will auto-resize the texture dimensions.
        }
        return tex;
        //Destroy(tex);
    }
}


