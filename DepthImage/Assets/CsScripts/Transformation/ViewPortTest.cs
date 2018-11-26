//using System.Collections;
//using System.Collections.Generic;
using UnityEngine;

public class ViewPortTest : MonoBehaviour {
    // Use this for initialization
    GameObject cube;
    Vector3 pos;
    Vector3 translation = new Vector3(7.145f,5.245f,6.196f);
    Vector3 rotation = new Vector3(23, 85, 13);
    public Vector3 scale = new Vector3(1, 1, 1);
    /*
     1.transformation t 를곱해준다
     2. 프로젝션 백 프로젝션으 한다. 
     3. transformation t  inverse 를 곱해준다
     4. 두점사이의 거리 displacement를 구한다.
   */

    void Start () {
		cube = GameObject.Find("Cube");

        Vector3 defaultPosition = cube.transform.localPosition;
        Vector3 defaultRotation = cube.transform.localEulerAngles;
        Debug.Log("default position: " + defaultPosition);
        //Debug.Log("default position: " + defaultRotation);
        
        // 1
        Quaternion rq = Quaternion.Euler(rotation.x,rotation.y,rotation.z);
        Matrix4x4 m = Matrix4x4.TRS(translation, rq, scale);
        var result = m.MultiplyPoint(cube.transform.position);
        cube.transform.position = result;
        //2  || word to CamaraMetrix 
        Vector3 w2s = Camera.main.WorldToScreenPoint(cube.transform.position);
        Vector3 s2w = Camera.main.ScreenToWorldPoint(w2s);

        //3
        Matrix4x4 m_inv = m.inverse;
        Vector3 moveBack = m_inv.MultiplyPoint(s2w);

        Debug.Log(moveBack.x);

        //4

        float displacement = Vector3.Distance(defaultPosition,moveBack);
        Debug.Log("displacement: "+displacement);
        cube.transform.position = moveBack;
    }
	
	// Update is called once per frame
	void Update () {        
    }
}
