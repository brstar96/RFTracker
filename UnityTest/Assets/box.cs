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
        ns = tcpclient.GetStream();
    }

    void Update() {
        ReceiveMessage = new byte[1024];
        ns.Read(ReceiveMessage, 0, 1024);
        StrReceiveMessage = Encoding.ASCII.GetString(ReceiveMessage);
        Debug.Log("strRecevieMessageis " + StrReceiveMessage);
        string[] T = StrReceiveMessage.Split(',');        
        
        string tx = T[0];
        string ty = T[1];
        string tz = T[2];


        string rx = T[3];
        string ry = T[4];
        string rz = T[5];



        transform.position = new Vector3(float.Parse(tx), float.Parse(ty), float.Parse(tz));
        transform.localEulerAngles = new Vector3(float.Parse(rx), float.Parse(ry), float.Parse(rz));
        
    }
}
