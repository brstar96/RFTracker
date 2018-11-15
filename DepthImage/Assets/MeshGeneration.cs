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

        List<Vector2> list = new List<Vector2>();

        Texture2D GrayscaleImg = new Texture2D(512, 512, TextureFormat.RGBA32, false);
        byte[] bytes = new byte[512 * 512];
        Texture2D testtex = LoadPNG(PATH);
        for (int height = 0; height < 512; height++)
        {
            for (int width = 0; width < 512; width++)
            {
                Color pixcels = testtex.GetPixel(height, width);
                if (pixcels.r > 0)
                {
                    list.Add(new Vector2(height, width));
                }
            }
        }
        for (int i =0;i<sizeof(); i++)
        {
        Debug.Log(list[3]);
        }
    }
            
    public Texture2D LoadPNG(string filePath)
    {
        Texture2D tex = null;
        byte[] fileData;
        
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


