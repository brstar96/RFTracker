using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class bunny_Scrpt : MonoBehaviour {
    private Transform m_Transform;
    public float speed = 1;
    void Start () {

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

    }
}
