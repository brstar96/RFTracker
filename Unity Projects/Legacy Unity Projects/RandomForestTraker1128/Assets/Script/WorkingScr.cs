using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System;
using System.IO;

/*
1. bunny에 t를 곱하고
2. 그 그 뎁스 이미지를 받아 프론트 페이스 만들고
3. 매 하나의 프론트 페이스 에서 20개의 점을 선정 --Xj
4. Xj에  t를 곱한다.
4. word to screen
5. screen to word
6. t 의 inverse 를 곱한다. 
7. 돌아온 점과 원래 점의 위치의 평균치를 구한다 .
8. 파일에 입력한다.
*/

public class WorkingScr : MonoBehaviour
{

    public Material mat;
    public int width = 512;
    public int height = 512;

    private Camera cam;
    private RenderTexture rt;
    private int image_id = 0;
    private bool SnapFlag = false;
    String[] BunnyPositionFileLines,colums,randomizeFileList;
    List<Vector3> grayScaleList, grayScalePixcels;
    List<GameObject> SphereList = new List<GameObject>(); //게임씬에 Sphere를 생성하기 위한 리스트
    GameObject bunny;
    Quaternion quatenion;
    Matrix4x4 matrix;
    List<GameObject> selectedSpheres, tempList;  
    float displacementSum , displacement= 0;
    StreamWriter file;
    List<String> strList;
    void Start()
    {
        
        cam = GetComponent<Camera>();   //Get camera component
        cam.depthTextureMode = DepthTextureMode.Depth;
        rt = new RenderTexture(width, height, 32);  // Create 32 bit depth texture object 
        cam.targetTexture = rt;
        SnapFlag = true;
        bunny = GameObject.Find("Bunny");
        StartCoroutine("BunnyMove");
        strList= new List<string>();
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

   void  BunnyMove(){//1 .
        BunnyPositionFileLines = File.ReadAllLines("./RandomizeTest1.csv");
        for (int BunnyPositionLineNo = 0; BunnyPositionLineNo < 1/*default = 642*/; BunnyPositionLineNo++)
        {
            colums = BunnyPositionFileLines[BunnyPositionLineNo].Split(',');
            bunny.transform.position = new Vector3(float.Parse(colums[0]) / 10, float.Parse(colums[1]) / 10, float.Parse(colums[2]) / 10);
            bunny.transform.localEulerAngles = new Vector3(float.Parse(colums[3]), float.Parse(colums[4]), float.Parse(colums[5])); //?
            StartCoroutine(createFrontFace());
          //  yield return new WaitForEndOfFrame();// 렌데링이 끝날 때까지
        }
        //Debug.Log(strList.Count);
        
    }
    //2. 
    IEnumerator createFrontFace()
    {
       SnapFlag = true;
       yield return new WaitForEndOfFrame();//렌더링이 끝날 때까지(OnRenderImage가 끝날 때까지인듯)

        grayScaleList = loadGrayScalePNG("./frontface.png");
        List<Vector3> vertexPositions = new List<Vector3>();
        GameObject singleSphere;

        SphereList = new List<GameObject>();
        for (int j = 0; j < grayScaleList.Count; j++)
        {
            singleSphere =(GameObject.CreatePrimitive(PrimitiveType.Sphere));
            singleSphere.transform.localScale = new Vector3(0.0009f, 0.0009f, 0.0009f);
            SphereList.Add(singleSphere);
        }

        for (int count = 0; count < grayScaleList.Count; count++)
        {
            vertexPositions.Add(new Vector3(grayScaleList[count].x / 512f, grayScaleList[count].y / 512f, grayScaleList[count].z / 2.55f));
        }

        for (int j = 0; j < SphereList.Count; j++)
        {
            SphereList[j].transform.position = new Vector3(vertexPositions[j].x - 0.5f, vertexPositions[j].y - 0.5f, vertexPositions[j].z * 2.5f);
        }
        selectedSpheres = selectTweentyPoints(SphereList); //받아온 프론트 페이스 점들에서 20개를 선택한다.  


        multiplyTau(selectedSpheres);
        
        //for(int i = 0; i < SphereList.Count; i++)
        //{
        //    Destroy(SphereList[i]);
        //}

    }
    void multiplyTau(List<GameObject> objs){
    
        randomizeFileList = File.ReadAllLines("./RandomizeTest1.csv");
        
        for (int randomNo = 0; randomNo < 2500/*2500개 tau*/; randomNo++)
        {
            colums = randomizeFileList[randomNo].Split(',');
            for (int objNo = 0; objNo < objs.Count; objNo++)
            {
                Vector3 defaultPosition = objs[objNo].transform.localPosition;
                quatenion = Quaternion.Euler(float.Parse(colums[3]), float.Parse(colums[4]), float.Parse(colums[5]));
                matrix = Matrix4x4.TRS(new Vector3(float.Parse(colums[0]), float.Parse(colums[1]), float.Parse(colums[2])),quatenion,new Vector3(1,1,1));
                Vector3 multiply_Vector3 = matrix.MultiplyPoint(objs[objNo].transform.localPosition);

                Vector3 w2s = cam.WorldToScreenPoint(multiply_Vector3);
                Vector3 s2w = cam.ScreenToWorldPoint(w2s);

                Matrix4x4 m_inv = matrix.inverse;
                Vector3 moveback = m_inv.MultiplyPoint(s2w);
                displacement = Vector3.Distance(defaultPosition,moveback);
                displacementSum =+ displacement;
            }
            float avg = displacementSum / 20;
            string str = avg + "," + randomizeFileList[randomNo];
            strList.Add(str);
                 
        }
        file = new System.IO.StreamWriter("./test.csv");
        for (int a = 0;a<strList.Count;a++)
        {
            file.WriteLine(strList[a]);
        }
        
        file.Close();

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
    //3.
    List<GameObject> selectTweentyPoints(List<GameObject> SphereList)
    {
        tempList = new List<GameObject>();
        System.Random random = new System.Random();
        for (int randomCount = 0; randomCount < 20; randomCount++)
        {
            tempList.Add(SphereList[random.Next(0, SphereList.Count)]);
        }
        return tempList;
    }

}
