using UnityEngine;
using System.Collections.Generic;
using System.IO;

[RequireComponent(typeof(MeshFilter))]
[RequireComponent(typeof(MeshRenderer))]

public class MeshGeneration : MonoBehaviour
{
    
    void Start()
    {
        CreateCube();
    }

    private void CreateCube()
    {
        Vector3 []vertices = new Vector3[20];

        for (int i = 0; i < 20; i++) {
            vertices[i] = new Vector3(0,0,0);
        }

        //Texture2D GrayscaleImg = new Texture2D(512, 512, TextureFormat.RGBA32, false);
        //string PATH = "DepthSampleImgs/31.png";
        //GrayscaleImg = Resources.Load(PATH, typeof(Texture2D)) as Texture2D;
        //byte[] bytes = new byte[512*512];
        //GrayscaleImg.LoadImage(bytes);

        //for(int i = 0; i < 512; i++)
        //{
        //    Debug.Log(bytes[i]);
        //}


        //Stream imageStreamSource = new FileStream("smiley.png", FileMode.Open, FileAccess.Read, FileShare.Read);
        //PngBitmapDecoder decoder = new PngBitmapDecoder(imageStreamSource, BitmapCreateOptions.PreservePixelFormat, BitmapCacheOption.Default);
        //BitmapSource bitmapSource = decoder.Frames[0];




        //Vector3[] vertices = {
        //    new Vector3 (1, 1, 1),
        //    new Vector3 (1, 0, 0),
        //    new Vector3 (1, 1, 0),
        //    new Vector3 (0, 1, 0),
        //    new Vector3 (0, 1, 1),
        //    new Vector3 (1, 1, 1),
        //    new Vector3 (1, 0, 1),
        //    new Vector3 (0, 0, 1),
        //};
        string []lines = File.ReadAllLines("D:/DepthTestImgs/196.png");
        
        Debug.Log(lines[0]);

        int[] triangles = {
            0, 2, 1, //face front
			0, 3, 2,
            2, 3, 4, //face top
			2, 4, 5,
            1, 2, 5, //face right
			1, 5, 6,
            0, 7, 4, //face left
			0, 4, 3,
            5, 4, 7, //face back
			5, 7, 6,
            0, 6, 7, //face bottom
			0, 1, 6
        };

        int[] Test ={
            0,1,2,
        };

        Mesh mesh = GetComponent<MeshFilter>().mesh;
        mesh.Clear();
        //mesh.vertices = vertices;
        //mesh.triangles = triangles;
        mesh.vertices = vertices;
        mesh.triangles= Test;
       

    }
}