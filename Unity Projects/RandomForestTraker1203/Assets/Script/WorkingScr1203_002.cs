//WorkingScr1203_002 - 두 개의 코루틴이 분리된 상태에서 2500번 Randomize 수행 후 20개의 Displacement를 구하고 Dataset을 만드는 코드 
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System;
using System.IO;

public class WorkingScr1203_002 : MonoBehaviour
{
    public Material mat;
    public int width = 512;
    public int height = 512;
    private Camera cam;
    private RenderTexture rt;
    private int image_id = 0;
    private bool SnapFlag = false;
    float displacementSum, displacement = 0;
    String[] BunnyRandomizeCount, colums, randomizeFileList;
    List<Vector3> grayScaleList, grayScalePixcels, vertexPositions, tempList, TInv_Multiplied, TDxj_list, Dxj_list_forVisualization;
    List<GameObject> SphereList = new List<GameObject>(); //게임씬에 Sphere를 생성하기 위한 리스트
    GameObject StanfordBunny;
    Matrix4x4 matrix;
    StreamWriter file;
    List<String> strList;
    string[] T_FileLInes;

    void Start()
    {
        
        cam = GetComponent<Camera>();
        cam.depthTextureMode = DepthTextureMode.Depth;
        rt = new RenderTexture(width, height, 32);  // 32 bit depth
        cam.targetTexture = rt;
        SnapFlag = true;
        StanfordBunny = GameObject.Find("StanfordBunny");
        strList = new List<string>();
        BunnyMove();
    }

    void BunnyMove()
    {//1 .
        Boolean IsBunnyMove = false; //토끼의 이동 유무(true = 이동, false = 원점에 고정)

        if (IsBunnyMove == true)
        {//각 뷰포인트마다 토끼를 움직일 경우 실행하는 구문. 
            T_FileLInes = File.ReadAllLines("./Assets/DataFiles/TauRandomize1.csv");

            //CameraPosition = Geodesic Grid의 Vertex 개수(=Nv viewpoints, default = 642)
            for (int CameraPosition = 0; CameraPosition < 1; CameraPosition++)
            {
                for (int RandomizeCount = 0; RandomizeCount < 2500; RandomizeCount++)
                {
                    colums = T_FileLInes[RandomizeCount].Split(',');
                    //토끼를 Random transformation하는 부분
                    StanfordBunny.transform.position = new Vector3(float.Parse(colums[0]) / 10, float.Parse(colums[1]) / 10, float.Parse(colums[2]) / 10);
                    StanfordBunny.transform.localEulerAngles = new Vector3(float.Parse(colums[3]) / 10, float.Parse(colums[4]) / 10, float.Parse(colums[5]) / 10);
                    StartCoroutine(CreateFrontfaceAndDisplacement()); //FF를 만들고 displacement와 Dataset을 만드는 코루틴
                }
            }
        }
        else
        {           //CameraPosition = Geodesic Grid의 Vertex 개수(=Nv viewpoints, default = 642)
                    StanfordBunny.transform.position = new Vector3(0, 0, 0);
                    StanfordBunny.transform.localEulerAngles = new Vector3(0, 0, 0);
                    StartCoroutine(CreateFrontfaceAndDisplacement()); //FF를 만들고 displacement와 Dataset을 만드는 코루틴
                    //CreateFrontfaceAndDisplacement(); //FF를 만들고 displacement와 Dataset을 만드는 함수 
        }
    }
    //void CreateFrontfaceAndDisplacement()
    IEnumerator CreateFrontfaceAndDisplacement()
    {//2.
        GameObject.Find("StanfordBunny").SetActive(true); //false to hide Gameobject, true to show
        vertexPositions = new List<Vector3>();  // x, y, grayDepth
        SnapFlag = true;
        yield return new WaitForEndOfFrame();//렌더링이 끝날 때까지
        grayScaleList = loadGrayScalePNG("./frontface.png");
        
        List<Vector3> XjPositions = new List<Vector3>();  // GT 3D positions
        List<Vector3> xjPositions = new List<Vector3>();  // T inverse를 곱해 임의의 위치로 보낸 Positions  

        //① 20 pixels of nj 구하기
        List<Vector3> nj_pixels = selectTwentyPoints(grayScaleList);//1번에 있는  selected Nj pixels  2D

        //② Xj Positions 구하기
        for (int i = 0; i < nj_pixels.Count; i++)
        {
            Vector3 cubePosition = cam.ScreenToWorldPoint(nj_pixels[i]); // 임시변수에 20 개의 front face를 생성해 저장
            XjPositions.Add(cubePosition); //Backprojection과정에 사용할 XjPositions 리스트에 cubePosition 저장
        }

        List<Vector3> temp_XjPositions = XjPositions; //20개의 Xj를 복제
        List<string> strList = new List<string>();
        
        for (int CameraPosition = 0; CameraPosition < 1; CameraPosition++)
        {//각 뷰포인트마다 토끼를 움직이지 "않을" 경우 실행하는 구문. 
            T_FileLInes = File.ReadAllLines("./Assets/DataFiles/TauRandomize" + (CameraPosition + 1) + ".csv"); 

            for (int RandomizeCount = 0; RandomizeCount < 2500; RandomizeCount++) //default = 2500
            {
                colums = T_FileLInes[RandomizeCount].Split(',');
               
                //T행렬을 정의하기 위한 Randomized Parameters를 csv로부터 읽어옴
                Vector3 translation = new Vector3(float.Parse(colums[0]), float.Parse(colums[1]), float.Parse(colums[2]));
                Vector3 rotation = new Vector3(float.Parse(colums[3]), float.Parse(colums[4]), float.Parse(colums[5]));
                //③, ④ xj positions와 TDxj Positions 구하기
               xjPositions = multiply_InvT_And_Projection(translation, rotation, temp_XjPositions); //T Inverse를 곱하고 Proj하되, T Inverse의 값만 리턴받아 xjPositions라고 함. 
               TDxj_list = Backproj_and_multiplyT(translation, rotation, nj_pixels); //nj_pixels를 Back-proj한 후 T를 곱해 TDxj_list를 리턴받음.
               Dxj_list_forVisualization = Backproj(translation, rotation, nj_pixels); //20 of xj를 시각화하기 위한 부분
            //⑤ Displacement 20개 구하기 (str = 20 of displacement and 6 of transformation parementers)
            string str = "";
            for (int j = 0; j < 20; j++)
            {
                str += Vector3.Distance(XjPositions[j], TDxj_list[j]).ToString() + ",";
            }
            for (int k = 0; k < 6; k++)
            {
                str += colums[k] + ",";
            }
            strList.Add(str);
            }
            StreamWriter file = new StreamWriter("D:/RFTracker/Unity Projects/RandomForestTraker1203/Assets/Datasets/DisplacementWithTau" + CameraPosition+".csv");
            for (int i =0;i<strList.Count;i++)
            {
                file.WriteLine(strList[i]);
            }
            file.Close();
        }
        
        //시각화 및 테스트문자열 출력함수
        TestAndVisualization(XjPositions, xjPositions, Dxj_list_forVisualization, TDxj_list);
        GameObject.Find("StanfordBunny").SetActive(false); //false to hide Gameobject, true to show
    }

    //각 Xj에 Randomized tau의 Inverse를 곱하고 Proj한 결과를 inversedList 리스트로 반환
    List<Vector3> multiply_InvT_And_Projection(Vector3 t, Vector3 r, List<Vector3> moved)
    {
        List<Vector3> inversedList = new List<Vector3>();
        Quaternion quaternion = Quaternion.Euler(r);
        matrix = Matrix4x4.TRS(new Vector3(t.x / 10.0f, t.y / 10.0f, t.z / 10.0f), quaternion, new Vector3(1, 1, 1));
        Matrix4x4 m_inv = matrix.inverse;
        for (int d = 0; d < moved.Count; d++)
        {
            Vector3 TInv_Multiplied;
            TInv_Multiplied = m_inv.MultiplyPoint(moved[d]);
            inversedList.Add(TInv_Multiplied);
        }
        return inversedList;
    }

    //nj_pixels를 Back-proj한 후 T를 곱해 TDxj_list를 리턴받음. 
    List<Vector3> Backproj_and_multiplyT(Vector3 t, Vector3 r, List<Vector3> moved)
    {
        //testList.Add(new List<Vector3>()); //두번째 리스트 초기화

        List<Vector3> T_MultipliedList = new List<Vector3>();
        Vector3 Backproj_multiply_Vector3, T_Multiplied;
        Quaternion quaternion = Quaternion.Euler(r);
        matrix = Matrix4x4.TRS(new Vector3(t.x / 10.0f, t.y / 10.0f, t.z / 10.0f), quaternion, new Vector3(1, 1, 1));

        for (int index = 0; index < moved.Count; index++)
        {
            Backproj_multiply_Vector3 = cam.ScreenToWorldPoint(moved[index]); //Back-Proj수행
            T_Multiplied = matrix.MultiplyPoint(Backproj_multiply_Vector3); //T를 곱해줌. 
            T_MultipliedList.Add(new Vector3(T_Multiplied.x * 2, T_Multiplied.y * 2, T_Multiplied.z + 0.5f));
        }

        return T_MultipliedList;
    }

    //nj_pixels를 Back-proj한 후 BackProjList를 리턴받음. 
    List<Vector3> Backproj(Vector3 t, Vector3 r, List<Vector3> moved)
    {
        List<Vector3> BackProjList = new List<Vector3>();
        List<Vector3> T_MultipliedList = new List<Vector3>();
        Vector3 Backproj_multiply_Vector3;
        Quaternion quaternion = Quaternion.Euler(r);
        matrix = Matrix4x4.TRS(new Vector3(t.x / 10.0f, t.y / 10.0f, t.z / 10.0f), quaternion, new Vector3(1, 1, 1));

        for (int index = 0; index < moved.Count; index++)
        {
            Backproj_multiply_Vector3 = cam.ScreenToWorldPoint(moved[index]); //Back-Proj수행
            BackProjList.Add(new Vector3(Backproj_multiply_Vector3.x * 2, Backproj_multiply_Vector3.y * 2, Backproj_multiply_Vector3.z + 0.5f));
        }

        return BackProjList;
    }

    void TestAndVisualization(List<Vector3> XjPositions, List<Vector3> xjPositions, List<Vector3> Dxj_list_forVisualization, List<Vector3> TDxj_list)
    {
        //시각화를 위한 for문들
        for (int Cube_XjCount = 0; Cube_XjCount < 20; Cube_XjCount++)
        {
            //Xj Cube를 생성하고 빨강색으로 렌더링 
            GameObject Cube_Xj = GameObject.CreatePrimitive(PrimitiveType.Cube);
            Cube_Xj.transform.localScale = new Vector3(0.0022f, 0.0022f, 0.0022f);
            Cube_Xj.transform.localPosition = new Vector3(2 * XjPositions[Cube_XjCount].x, 2 * XjPositions[Cube_XjCount].y, 2 * (XjPositions[Cube_XjCount].z + 0.5f));
            Cube_Xj.GetComponent<MeshRenderer>().material.color = new Color((float)1.0f, (float)0.0f, (float)0.0f);
        }
        for (int Cylinder_xjCount = 0; Cylinder_xjCount < 20; Cylinder_xjCount++)
        {
            //xj Cylinder를 생성하고 초록색으로 렌더링 
            GameObject Cylinder_xj = GameObject.CreatePrimitive(PrimitiveType.Cylinder);
            Cylinder_xj.transform.localScale = new Vector3(0.0018f, 0.0009f, 0.0018f);
            Cylinder_xj.transform.localPosition = new Vector3(xjPositions[Cylinder_xjCount].x, xjPositions[Cylinder_xjCount].y, xjPositions[Cylinder_xjCount].z);
            Cylinder_xj.GetComponent<MeshRenderer>().material.color = new Color((float)0.0f, (float)1.0f, (float)0.0f);
        }
        for (int Cylinder_DxjCount = 0; Cylinder_DxjCount < 20; Cylinder_DxjCount++)
        {
            //Dxj Cylinder를 생성하고 분홍색으로 렌더링
            GameObject Cylinder_Dxj = GameObject.CreatePrimitive(PrimitiveType.Cylinder);
            Cylinder_Dxj.transform.localScale = new Vector3(0.0036f, 0.0036f, 0.0036f);
            Cylinder_Dxj.transform.localPosition = new Vector3(Dxj_list_forVisualization[Cylinder_DxjCount].x, Dxj_list_forVisualization[Cylinder_DxjCount].y, Dxj_list_forVisualization[Cylinder_DxjCount].z);
            Cylinder_Dxj.GetComponent<MeshRenderer>().material.color = new Color((float)1.0f, (float)0.0f, (float)1.0f);
        }
        for (int Sphere_TDxjCount = 0; Sphere_TDxjCount < 20; Sphere_TDxjCount++)
        {
            //TD(xj) Sphere를 생성하고 파랑색으로 렌더링 
            GameObject Sphere_TDxj = GameObject.CreatePrimitive(PrimitiveType.Sphere);
            Sphere_TDxj.transform.localScale = new Vector3(0.0036f, 0.0036f, 0.0036f);
            Sphere_TDxj.transform.localPosition = new Vector3(TDxj_list[Sphere_TDxjCount].x, TDxj_list[Sphere_TDxjCount].y, TDxj_list[Sphere_TDxjCount].z);
            Sphere_TDxj.GetComponent<MeshRenderer>().material.color = new Color((float)0.0f, (float)0.0f, (float)1.0f);
        }
        for (int PositionCount = 0; PositionCount < 20; PositionCount++)
        {
            //각 포지션들을 텍스트로 출력
        //    Debug.Log(PositionCount + "th XjPositions -" + " x: " + XjPositions[PositionCount].x + " y: " + XjPositions[PositionCount].y + " z: " + XjPositions[PositionCount].z);
        //    Debug.Log(PositionCount + "th xjPositions -" + " x: " + xjPositions[PositionCount].x + " y: " + xjPositions[PositionCount].y + " z: " + xjPositions[PositionCount].z);
        //    Debug.Log(PositionCount + "th TDxj_list -" + " x: " + TDxj_list[PositionCount].x + " y: " + TDxj_list[PositionCount].y + " z: " + TDxj_list[PositionCount].z);
        }
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
            savePNG(image, "D:/DepthTestImgs/"+image_id+".png");
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
}
