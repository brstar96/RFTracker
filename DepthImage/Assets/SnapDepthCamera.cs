using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.IO;


public class SnapDepthCamera : MonoBehaviour {
    public Material mat;
    private Camera cam;
    private RenderTexture rt;
    private GameObject bunny;
    public Transform target;

    public int width = 512;
    public int height = 512;
    private int image_id = 0;
    private bool SnapFlag = false;
    public string[] files;
    public string[] lines;
    StreamWriter writer;

    // Use this for initialization
    void Start () {
        /**
         * 뎁스 이미지 받기
         * 
         * **/
        cam = GetComponent<Camera>();   //获取当前绑定到脚本的相机
        cam.depthTextureMode = DepthTextureMode.Depth;
        rt = new RenderTexture(width, height, 24);  // 24 bit depth
        cam.targetTexture = rt;
        writer = new StreamWriter(@"./Assets/datasets.csv");     
        bunny = GameObject.Find("StanfordBunny");
        writer.WriteLine("displacement,Tx,Ty,Tz,Rx,Ry,Rz");
        StartCoroutine("RunMove");

    }

    void OnRenderImage(RenderTexture source, RenderTexture destination)
    {
        Graphics.Blit(source, destination, mat);

        if (SnapFlag)
        {
            RenderTexture currentRT = RenderTexture.active;   
            RenderTexture.active = destination;

            Texture2D image = new Texture2D(destination.width, destination.height, TextureFormat.RGB24, false);
            image.ReadPixels(new Rect(0, 0, destination.width, destination.height), 0, 0);
            image.Apply();
            savePNG(image, "D:/RFTracker/DepthImage/Assets/DepthImagesStore/" + image_id + ".png");
            image_id++;
            RenderTexture.active = currentRT; // restore 
        }
    }


    private void savePNG(Texture2D image, string path_file)
    {
        // store the texture into a .PNG file
        byte[] bytes = image.EncodeToPNG();
       

        // save the encoded image to a file
        System.IO.File.WriteAllBytes(path_file, bytes);
        SnapFlag = false;
        Debug.Log("randomize file saved");




    }	


	void Update () {
        transform.LookAt(target);
    }

    IEnumerator RunMove()
    {
        for (int fileNo = 0; fileNo < 1; fileNo++)
        {
            lines = File.ReadAllLines("D:/RFTracker/DepthImage/Assets/RandomizedFiles/RandomizeTest" + (fileNo + 1) + ".csv");
            for (int lineNo = 0; lineNo < 10; lineNo++)
            {
                string[] colums = lines[lineNo].Split(',');
               
               
                bunny.transform.position = new Vector3(float.Parse(colums[0]), float.Parse(colums[1]), float.Parse(colums[2]));
                writer.WriteLine("displacement," + colums[0] + "," + colums[1] + "," + colums[2] + "," + colums[3] + "," + colums[4] + "," + colums[5] + "," + colums[6]);
                bunny.transform.localEulerAngles = new Vector3(float.Parse(colums[3]), float.Parse(colums[4]), float.Parse(colums[5]));
                Debug.Log("file Name : RandomizeTest" + (fileNo + 1) + ".csv " + "lineNo : " + lineNo + "translate : " + float.Parse(colums[0]) + "," + colums[1] + "," + colums[2] + "loate : " + float.Parse(colums[3]) + "," + colums[4] + "," + colums[5] + "RowNumber : " + colums[6]);
                SnapFlag = false;
                yield return new WaitForSeconds(0.00005f);
            }           

        }


        writer.Close();  
    }
}
