using UnityEngine;
using System.Collections.Generic;
using System.IO;

public class MeshGeneration : MonoBehaviour
{
    public List<Vector3> _Vertex = new List<Vector3>(); 
    public List<int> _Tri = new List<int>();
    //public List<Vector2> _UV = new List<Vector2>(); //UV좌표 저장을 위한 vector2 변수
    private Mesh mesh;
    
    string PATH = "D:/DepthTestImgs/0.png";

    void Start()
    {
        CreateCube();
    }

    private void CreateCube()
    {
        //allocate 20 Vector3 variables('vertices') for vertex making
        Vector3[] vertices = new Vector3[1800]; //랜덤하게 버텍스 20개를 뽑기 위한 변수 
        List<GameObject> SphereList = new List<GameObject>(); //backprojected sphere 오브젝트를 만들기 위한 변수 
        GameObject ins; //sphere 게임오브젝트 인스턴스 생성을 위한 변수 
        List<Vector3> grayScalePixcels = new List<Vector3>(); 
        List<Vector3> vertexPositions = new List<Vector3>(); 
        
        //backprojected sphere 만드는 루프문(for visualization)
        for (int i = 0; i < SphereList.Count; i++)
        {
            ins = GameObject.CreatePrimitive(PrimitiveType.Sphere); //sphere생성(10개)++
            ins.transform.localScale = new Vector3(0.0009f,0.0009f,0.0009f); ;
            SphereList.Add(ins);
            vertices[i] = new Vector3(0, 0, 0);
        }
        
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
            //vertexPositions.Add(new Vector3(grayScalePixcels[count].x, grayScalePixcels[count].y, grayScalePixcels[count].z));
            vertexPositions.Add(new Vector3(grayScalePixcels[count].x/512f, grayScalePixcels[count].y / 512f, grayScalePixcels[count].z / 5.12f));
        }

        for (int i = 0; i < SphereList.Count; i++)
        {
            //i번째 sphere를 backprojected position으로 transform
            SphereList[i].transform.position = new Vector3(vertexPositions[i].x, vertexPositions[i].y, vertexPositions[i].z*5);
            getDisplacement(); //call 'getDisplacement' function for calculate displacement value per 'GT-backprojected vertex pair'. (11.18)
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
    public float getDisplacement(Vector3 v1, Vector3 v2)
    {
        //v1 : Ground truth vertex position from original 'stanfordbunny.obj' file.
        //v2 : back-projected vertex position from 'BackprojTest' unity scene.
        //tree will learn the relationship between displacement scalar value & corresponding transformation parameter tau.
        float tempDisplacement = 0;
        
        tempDisplacement = Vector3.Distance(v1, v2);
        return tempDisplacement;
    }


}


