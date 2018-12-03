//1202 - Rotation이 들어가지 않은 상태로 WorldtoScreen과 ScreentoWorld함수 실행한 스크립트(결과스샷 1203_noRotation.jpg 참고)
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System;
using System.IO;

public class WorkingScr1202 : MonoBehaviour{
    public Material mat;
    public int width = 512;
    public int height = 512;
    private Camera cam;
    private RenderTexture rt;
    private int image_id = 0;
    private bool SnapFlag = false;
    String[] BunnyPositionFileLines, colums, randomizeFileList;
    List<Vector3> grayScaleList, grayScalePixcels, vertexPositions, tempList;
    List<GameObject> SphereList = new List<GameObject>(); //게임씬에 Sphere를 생성하기 위한 리스트
    GameObject StanfordBunny;
    Matrix4x4 matrix;
    List<GameObject> selectedSpheres;
    float displacementSum, displacement = 0;
    StreamWriter file;
    List<String> strList;
    List<Vector3> TDxj_list;
    void Start()
    {
        
        
        cam = GetComponent<Camera>();   //获取当前绑定到脚本的相机
        cam.depthTextureMode = DepthTextureMode.Depth;
        rt = new RenderTexture(width, height, 24);  // 24 bit depth
        cam.targetTexture = rt;
        SnapFlag = true;
        StanfordBunny = GameObject.Find("StanfordBunny");
        StartCoroutine("BunnyMove");
        strList = new List<string>();
        
    }
    void OnRenderImage(RenderTexture source, RenderTexture destination)
    {
        Graphics.Blit(source, destination, mat);

        if (SnapFlag)
        {
            RenderTexture currentRT = RenderTexture.active;   // save current active rendertexture
            RenderTexture.active = destination;
            Texture2D image = new Texture2D(destination.width, destination.height, TextureFormat.RGB24, false);
            image.ReadPixels(new Rect(0, 0, destination.width, destination.height), 0, 0);
            image.Apply();
            savePNG(image, "./frontface.png");
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

    void BunnyMove()
    {//1 .
        BunnyPositionFileLines = File.ReadAllLines("./RandomizeTest1.csv");
        for (int BunnyPositionLineNo = 0; BunnyPositionLineNo < 1/*642*/; BunnyPositionLineNo++)
        {
            colums = BunnyPositionFileLines[BunnyPositionLineNo].Split(',');
           // bunny.transform.position = new Vector3(float.Parse(colums[0]) / 10, float.Parse(colums[1]) / 10, float.Parse(colums[2]) / 10);
            //bunny.transform.localEulerAngles = new Vector3(float.Parse(colums[3]) / 10, float.Parse(colums[4]) / 10, float.Parse(colums[5]) / 10);
            StartCoroutine(createFrontFace());
            //  yield return new WaitForEndOfFrame();// 렌데링이 끝날 때까지
        }
        //Debug.Log(strList.Count);
    }
    //2. 
    IEnumerator createFrontFace()
    {
        vertexPositions = new List<Vector3>();  // x y grayDepth
        SphereList = new List<GameObject>();

        SnapFlag = true;
        yield return new WaitForEndOfFrame();//렌데링이 끝날 때까지

        grayScaleList = loadGrayScalePNG("./frontface.png");

        List<Vector3> nj_pixels = selectTwentyPoints(grayScaleList);//1번에 있는  selected Nj pixels  2D
        List<Vector3> XjPositions = new List<Vector3>();  // 3D  

        List<Vector3> cubePositionList = new List<Vector3>();
        for (int i = 0; i < nj_pixels.Count; i++)
        {
            Vector3 cubePosition =  cam.ScreenToWorldPoint(nj_pixels[i]); // 20 개의 front face
            GameObject singleCube = GameObject.CreatePrimitive(PrimitiveType.Cube);
            singleCube.transform.localScale = new Vector3(0.0022f, 0.0022f, 0.0022f);
            singleCube.transform.localPosition = new Vector3(2 * cubePosition.x, 2 * cubePosition.y, 2 * (cubePosition.z + 0.5f));
            //Xj Cube를 빨강색으로 렌더링 
            singleCube.GetComponent<MeshRenderer>().material.color = new Color((float)1.0f, (float)0.0f, (float)0.0f);

            XjPositions.Add(cubePosition); //Backprojection과정에 사용
            cubePositionList.Add(cubePosition);
        }
        


        List<Vector3> temp_XjPositions = XjPositions; //20개의 Xj를 복제


        string[] T_FileLInes = File.ReadAllLines("./Assets/DataFiles/RandomizeTest1.csv");


        file = new System.IO.StreamWriter("./test.csv");
        for (int i = 0; i < 1; i++)
        {   
            string[] tranformations = T_FileLInes[i].Split(',');
            Vector3 translation = new Vector3(float.Parse(tranformations[0]), float.Parse(tranformations[1]), float.Parse(tranformations[2]));
           // Vector3 rotation = new Vector3(float.Parse(tranformations[3]), float.Parse(tranformations[4]), float.Parse(tranformations[5]));
            Vector3 rotation = new Vector3(0,0,0);
            List<Vector3> xjPositions = multiply_InvT_And_Projection(translation, rotation, temp_XjPositions); //xj를 연산
            //Debug.Log(new Vector3(translation.x, translation.y, translation.z));
            TDxj_list = Backproj_and_multiplyT(translation, rotation, nj_pixels);
            List<String> tempList = new List<String>();
            string str = "";
            //Debug.Log(TDxj_list.Count);
            for (int j = 0; j < 20; j++)
            {
                str += Vector3.Distance(XjPositions[j], TDxj_list[j]).ToString() + ",";
            }
            for (int k = 0; k < 6; k++)
            {
                str += tranformations[k] + ",";

            }
            file.WriteLine(str);
        }
            for (int x =0;x<20;x++)
        {
            GameObject Sphere = GameObject.CreatePrimitive(PrimitiveType.Sphere);
            Sphere.transform.localScale = new Vector3(0.0036f, 0.0036f, 0.0036f);
            Sphere.transform.localPosition = new Vector3(TDxj_list[x].x, TDxj_list[x].y, TDxj_list[x].z);
            //TD(xj)를 파란색으로 렌더링 
            Sphere.GetComponent<MeshRenderer>().material.color = new Color((float)0.0f, (float)0.0f, (float)1.0f);
        }
        file.Close();
        GameObject.Find("StanfordBunny").SetActive(false); //false to hide Gameobject, true to show.

        for (int i=0;i<20;i++)
        {
            Debug.Log("XjPositions: " + " x: " + XjPositions[i].x + " y: " + XjPositions[i].y + " z: " + XjPositions[i].z);
            Debug.Log("TDxj_list: " + " x: " + TDxj_list[i].x + " y: " + TDxj_list[i].y + " z: " + TDxj_list[i].z);
        }
    }


    /*
     1. 매 하나의 Xj 에 tau 의 inverse를 곱고 프로젝션을 수행한 결과를 
     리스트(xj)로 돌려준다. 
     */
    List<Vector3> multiply_InvT_And_Projection(Vector3 t, Vector3 r, List<Vector3> moved)
    {
        List<Vector3> inversedList = new List<Vector3>();
        Quaternion quaternion = Quaternion.Euler(r);
        matrix = Matrix4x4.TRS(new Vector3(t.x/10.0f,t.y/10.0f,t.z/10.0f), quaternion, new Vector3(1, 1, 1));
        Matrix4x4 m_inv = matrix.inverse;
        for (int d = 0; d < moved.Count; d++)
        {
            Vector3 mulplied = m_inv.MultiplyPoint(moved[d]);
            
            //xj를 시각화
            GameObject Cylinder = GameObject.CreatePrimitive(PrimitiveType.Cylinder);
            Cylinder.transform.localScale = new Vector3(0.0018f, 0.0009f, 0.0018f);
            Cylinder.transform.localPosition = new Vector3(mulplied.x, mulplied.y, mulplied.z);
            //xj를 초록색으로 렌더링 
            Cylinder.GetComponent<MeshRenderer>().material.color = new Color((float)0.0f, (float)1.0f, (float)0.0f);

            inversedList.Add(mulplied);
        }
        return inversedList;
    }

    List<Vector3> Backproj_and_multiplyT(Vector3 t, Vector3 r, List<Vector3> moved)
    {
        List<Vector3> BackProjList = new List<Vector3>();
        Vector3 Backproj_multiply_Vector3;
        Quaternion quaternion = Quaternion.Euler(r);
        Matrix4x4 Tmatrix = Matrix4x4.TRS(new Vector3(t.x /10.0f, t.y / 10.0f, t.z / 10.0f), quaternion, new Vector3(1, 1, 1));

        for (int index = 0; index < moved.Count; index++)
        {
            Backproj_multiply_Vector3 = cam.ScreenToWorldPoint(moved[index]);

            //BackProjList.Add(new Vector3(matrix.MultiplyPoint(Backproj_multiply_Vector3).x * 2, matrix.MultiplyPoint(Backproj_multiply_Vector3).y * 2, matrix.MultiplyPoint(Backproj_multiply_Vector3).z + 0.5f));
            BackProjList.Add(new Vector3(Backproj_multiply_Vector3.x*2, Backproj_multiply_Vector3.y*2, Backproj_multiply_Vector3.z+0.5f));
            
        }

        return BackProjList;
    }

    List<Vector3> selectTwentyPoints(List<Vector3> vec)
    {
        tempList = new List<Vector3>();
        System.Random random = new System.Random();
        for (int randomCount = 0; randomCount < 20; randomCount++)
        {
            tempList.Add(vec[random.Next(0, vec.Count)]);
        }
        return tempList;
    }

    
    List<Vector3> loadGrayScalePNG(string filePath)
    {
        grayScalePixcels = new List<Vector3>();
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
