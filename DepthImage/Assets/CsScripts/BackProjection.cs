using UnityEngine;
using System.Collections.Generic;
using System.IO;

public class BackProjection : MonoBehaviour
{
    string PATH = "D:/DepthTestImgs/0.png";
    
    void Start()
    {
        BackProj();
    }

    private void BackProj()
    {
        GameObject obj = GameObject.Find("empty");

        List<GameObject> SphereList = new List<GameObject>(); //20개의 sphere 오브젝트를 만들기 위한 변수
        CreateCube();
    }

    private void CreateCube()
    {
        List<GameObject> SphereList = new List<GameObject>();
        List<Vector3> vertexPositions = new List<Vector3>();
        List<Vector3> grayScalePixcels = loadGrayScalePNG(PATH); //loading image from 'PATH' and store to 'testtex' Texture2D variable. 
        GameObject singleSphere;
        for (int j = 0; j < grayScalePixcels.Count; j++)
        {
            singleSphere = GameObject.CreatePrimitive(PrimitiveType.Sphere);
            singleSphere.transform.localScale = new Vector3(0.0009f, 0.0009f, 0.0009f);
            SphereList.Add(singleSphere);
        }
        for (int count = 0; count < grayScalePixcels.Count; count++)
        {
            vertexPositions.Add(new Vector3(grayScalePixcels[count].x / 512f, grayScalePixcels[count].y / 512f, grayScalePixcels[count].z / 2.55f));
        }

        for (int j = 0; j < SphereList.Count; j++)
        {
            //SphereList[j].transform.position = new Vector3(vertexPositions[j].x, vertexPositions[j].y, vertexPositions[j].z);
            SphereList[j].transform.position = new Vector3(vertexPositions[j].x-0.5f, vertexPositions[j].y-0.5f, (vertexPositions[j].z * 2.5f)-0.5f);
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
                if (pixcels.r <1)
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
        //Debug.Log(Vector3.Distance(b, a));
        //Debug.Log(Vector3.Distance(a, b));
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


