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
        //allocate 20 Vector3 variables('vertices') for vertex making
        Vector3[] vertices = new Vector3[20];
        GameObject spheere = GameObject.Find("Sphere");
        //Iterate 20 times to store vertex data to 'verices' variable. 
        for (int i = 0; i < 20; i++)
        {
            vertices[i] = new Vector3(0, 0, 0);
        }

        List<Vector3> grayScalePixcels = new List<Vector3>();
        List<Vector3> vertexPositions = new List<Vector3>();

        Texture2D GrayscaleImg = new Texture2D(512, 512, TextureFormat.RGBA32, false); //allocate memory for 2D texture image
        byte[] bytes = new byte[512 * 512]; //allocate byte variable for Image
        Texture2D testtex = LoadPNG(PATH); //loading image from 'PATH' and store to 'testtex' Texture2D variable. 
        Color pixcels;


        //Iteration for 512*512 times
        for (int height = 0; height < 512; height++)
        {
            for (int width = 0; width < 512; width++)
            {
                pixcels = testtex.GetPixel(height, width);
                if (pixcels.r < 1) //this operation must be execute when 'pixcels' have non-zero value. 
                {
                    grayScalePixcels.Add(new Vector3(height, width, pixcels.r));
                }
            }
        }
        for (int count = 0; count < grayScalePixcels.Count; count++)
        {
            vertexPositions.Add(new Vector3(grayScalePixcels[count].x, grayScalePixcels[count].y, grayScalePixcels[count].z));
        }
        Debug.Log(grayScalePixcels[3].x + "  " + grayScalePixcels[3].y + "  " + grayScalePixcels[3].z);
        Debug.Log(vertexPositions[3].x/5.12 + "  " + vertexPositions[3].y/5.12 + "  " + vertexPositions[3].z/2.55);
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


