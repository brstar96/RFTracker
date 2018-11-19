﻿using UnityEngine;
using System.Collections.Generic;
using System.IO;

public class MeshGeneration : MonoBehaviour
{
    string PATH = "D:/DepthTestImgs/0.png";


    void Start()
    {
        BackProjection();
    }

    private void BackProjection()
    {
        GameObject obj = GameObject.Find("empty");

        List<GameObject> SphereList = new List<GameObject>(); //20개의 sphere 오브젝트를 만들기 위한 변수 
    }
       
    private void CreateCube()
    {
        //allocate 20 Vector3 variables('vertices') for vertex making
        Vector3[] vertices = new Vector3[1800]; //랜덤하게 버텍스 20개를 뽑기 위한 변수 
        List<GameObject> SphereList = new List<GameObject>(); //backprojected sphere 오브젝트를 만들기 위한 변수 

        GameObject ins; //sphere 게임오브젝트 인스턴스 생성을 위한 변수 
        List<Vector3> vertexPositions = new List<Vector3>();

        byte[] bytes = new byte[512 * 512]; //allocate byte variable for Image

        List<Vector3> grayScalePixcels = loadGrayScalePNG(PATH); //loading image from 'PATH' and store to 'testtex' Texture2D variable. 
        //20개 sphere 만드는 루프문
        for (int i = 0; i < grayScalePixcels.Count; i++)
        {


            //backprojected sphere 만드는 루프문(for visualization)
            for (int j = 0; j < SphereList.Count; j++)
            {

                ins = GameObject.CreatePrimitive(PrimitiveType.Sphere); //sphere생성(10개)++

                ins.transform.localScale = new Vector3(0.0009f, 0.0009f, 0.0009f);
                SphereList.Add(ins);

                vertices[j] = new Vector3(0, 0, 0);

            }


            for (int count = 0; count < grayScalePixcels.Count; count++)
            {
                vertexPositions.Add(new Vector3(grayScalePixcels[count].x / 512f, grayScalePixcels[count].y / 512f, grayScalePixcels[count].z / 5.12f));
            }

            for (int j = 0; j < SphereList.Count; j++)
            {
                //i번째 sphere를 backprojected position으로 transform

                SphereList[j].transform.position = new Vector3(vertexPositions[j].x, vertexPositions[j].y, vertexPositions[j].z);


                SphereList[j].transform.position = new Vector3(vertexPositions[j].x, vertexPositions[j].y, vertexPositions[j].z * 5);
                getDisplacement(); //call 'getDisplacement' function for calculate displacement value per 'GT-backprojected vertex pair'. (11.18)

            }
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

    public void getDisplacement()
    {
        Vector3 a = new Vector3(2, 2, 2);
        Vector3 b = new Vector3(1, 5, 9);
        Debug.Log(Vector3.Distance(b, a));
        Debug.Log(Vector3.Distance(a, b));
    }

    public float getDisplacement(Vector3 v1, Vector3 v2)
    {
        //v1 : Ground truth vertex position from original 'stanfordbunny.obj' file.
        //v2 : back-projected vertex position from 'BackprojTest' unity scene.
        //tree will learn the relationship between displacement scalar value & corresponding transformation parameter tau.
        float tempDisplacement = 0;

        tempDisplacement = Vector3.Distance(v1, v2);
        return tempDisplacement;

    }

    public void comunicationWithPython()
    {

    }
}


