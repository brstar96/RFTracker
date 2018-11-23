using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ViewPortTest : MonoBehaviour {
    // Use this for initialization
    GameObject cube;
    Vector3 pos;
    void Start () {
		pos = Camera.main.ViewportToWorldPoint(new Vector3(0.3f,0.3f,0.56f));
        cube = GameObject.Find("Cube");
        
	}
	
	// Update is called once per frame
	void Update () {
        if (Input.GetKey(KeyCode.Space))
        {
            cube.transform.position = pos;
            //Debug.Log(pos);
        }
    }
}
