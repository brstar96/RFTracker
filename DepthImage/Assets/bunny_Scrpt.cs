using System.Collections;
using System.Collections.Generic;
using UnityEngine;


public class bunny_Scrpt : MonoBehaviour {
    private Transform m_Transform;
    public float speed = 1;
    GameObject GameObjCama;
    Camera came;
    string[] files;
    public Material mat;
    private RenderTexture rt;
    private int image_id = 0;
    private bool SnapFlag=false;


    void Start () {
        GameObjCama = GameObject.Find("snap");
        came = GameObjCama.GetComponent<Camera>();
        rt = new RenderTexture(512, 512, 24);  // 24 bit depth
        came.targetTexture = rt;


        Debug.Log("call cam"+came);


        
    }




    /*
     *카메라와 같은 뷰 방향에서 
     * H J 키 로 상하 위치 조정  
     * W A S D 로 객체가 있는 위치에서 앞 뒤 좌 우로 이동
     * 
     */
    void FixedUpdate () {
        if (Input.GetKey(KeyCode.H))
        {
            this.transform.Translate(0, speed * Time.deltaTime*10, 0);
        }
        if (Input.GetKey(KeyCode.J))
        {
            this.transform.Translate(0, -speed * Time.deltaTime*10, 0);
        }


        if (Input.GetKey(KeyCode.W))
        {
            this.transform.Translate(0, 0, speed * Time.deltaTime*10);
        }
        if (Input.GetKey(KeyCode.S))
        {
            this.transform.Translate(0, 0, -speed * Time.deltaTime*10);
        }

        if (Input.GetKey(KeyCode.A))
        {
            this.transform.Translate(-speed * Time.deltaTime*10,0, 0);
        }
        if (Input.GetKey(KeyCode.D))
        {
            this.transform.Translate(speed * Time.deltaTime*10,0, 0);
        }

        if (Input.GetMouseButton(0))
        {
            
            this.transform.Rotate(5*Input.GetAxis("Mouse X"),0,0);
        }
        if (Input.GetMouseButton(1))
        {
            this.transform.Rotate(0,5 * Input.GetAxis("Mouse X"), 0);

        }
        if (Input.GetMouseButton(2))
        {
            this.transform.Rotate(0,0,5 * Input.GetAxis("Mouse X"));
        }

        if (Input.GetKeyDown("k"))
        {
            SnapFlag = true;
            Debug.Log("save ");
        }
        else
            SnapFlag = false;

    }


    void OnRenderImage(RenderTexture source, RenderTexture destination)
    {
        Graphics.Blit(source, destination, mat);
        //mat is the material which contains the shader
        //we are passing the destination RenderTexture to

        if (SnapFlag)
        {
            RenderTexture currentRT = RenderTexture.active;   // save current active rendertexture
            RenderTexture.active = destination;

            Texture2D image = new Texture2D(destination.width, destination.height, TextureFormat.RGB24, false);
            image.ReadPixels(new Rect(0, 0, destination.width, destination.height), 0, 0);
            image.Apply();

            savePNG(image, "D:/RFTracker/DepthImage/Assets/DepthImagesStore/bunnyCamera" + image_id + ".png");

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
    }
}
