using System.Collections;
using System.Collections.Generic;
using UnityEngine;

using System;
using System.Text;
using System.Net;
using System.Net.Sockets;

public class box : MonoBehaviour {

    TcpListener tcpListener = null;
    TcpClient tcpclient = null;
    NetworkStream ns = null;
    byte[] ReceiveMessage = null;
    String StrReceiveMessage = "";
    void Start () {
        tcpListener = new TcpListener(IPAddress.Parse("127.0.0.1"), 6789);
        tcpListener.Start();
        tcpclient = tcpListener.AcceptTcpClient();
        Debug.Log("대기 상태 시작!!");
        
        //ReceiveMessage = Encoding.Default.GetBytes("0,0,0,0,0,0");

        ns = tcpclient.GetStream();
        
            

        

    }

    void Update() {
        ReceiveMessage = new byte[1024];
        ns.Read(ReceiveMessage, 0, 1024);
        StrReceiveMessage = Encoding.ASCII.GetString(ReceiveMessage);
        Debug.Log("strRecevieMessageis " + StrReceiveMessage);
        //    
        //    ReceiveMessage = new byte[6];
        //    ns = tcpclient.GetStream();
        //    ns.Read(ReceiveMessage, 0, 6);
        //    StrReceiveMessage = Encoding.UTF8.GetString(ReceiveMessage);
        //    Debug.Log("strRecevieMessageis "+StrReceiveMessage);

        //string[] T = StrReceiveMessage.Split(',');
        //for (int a = 0; a < T.Length;  a++)
        //{
        //    Debug.Log(a+"="+T[a]);

        //}
        // Debug.Log("========================");
        //string tx = T[0];
        //string ty = T[1];
        //string tz = T[2];

        //string rx = T[3];
        //string ry = T[4];
        //string rz = T[5];



        //transform.position = new Vector3(float.Parse(tx), 0, 0);
        //transform.position = new Vector3(0,float.Parse(ty), 0);
        //transform.position = new Vector3(0,0,float.Parse(tz));

        //transform.localEulerAngles = new Vector3(float.Parse(rx), 0, 0);
        //transform.localEulerAngles = new Vector3(0,float.Parse(ry),0);
        //transform.localEulerAngles = new Vector3(0,0,float.Parse(rz));
    }
}
