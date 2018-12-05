using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.IO;
using System;

public class TrackingScr : MonoBehaviour {
    public Material mat;
    private Camera cam;    
    private GameObject GTPos, PredictedPos, DepthCamera;
    string[] colums;

    private string[] GTBunnyPositions, PredictedBunnyPositions;
    StreamWriter writer;

    // Use this for initialization
    void Start () {
        cam = GetComponent<Camera>();
        GTPos = GameObject.Find("GTPos");
        PredictedPos = GameObject.Find("PredictedPos");
        StartCoroutine("RunMove");
    }
	
	// Update is called once per frame
	void Update () {
     
    }

    //토끼를 이동하는 코루틴
    IEnumerator RunMove()
    {
        //GTPos(X_test) csv파일을 읽어서 GTPos토끼의 포지션을 변경해줌. 
        GTBunnyPositions = File.ReadAllLines("D:/RFTracker/Unity Projects/RandomForestTraker1203/Assets/DataFiles/gt.csv");
        PredictedBunnyPositions = File.ReadAllLines("D:/RFTracker/Unity Projects/RandomForestTraker1203/Assets/DataFiles/predicted.csv");

        for (int SimulationCount = 0; SimulationCount < 500; SimulationCount++)
        {
            //GTBunny의 Transformation을 변경하고 빨강색으로 렌더링. 
            colums = GTBunnyPositions[SimulationCount].Split(',');
            GTPos.transform.position = new Vector3(float.Parse(colums[0]) / 10, float.Parse(colums[1]) / 10, float.Parse(colums[2]) / 10);
            GTPos.transform.localEulerAngles = new Vector3(float.Parse(colums[3]), float.Parse(colums[4]), float.Parse(colums[5]));
            GTPos.GetComponent<MeshRenderer>().material.color = new Color((float)1.0f, (float)0.0f, (float)0.0f);
            //yield return new WaitForSeconds(0.1f);

            //PredictedBunny의 Transformation을 변경하고 파랑색으로 렌더링. 
            colums = PredictedBunnyPositions[SimulationCount].Split(',');
            PredictedPos.transform.position = new Vector3(float.Parse(colums[0]) / 10, float.Parse(colums[1]) / 10, float.Parse(colums[2]) / 10);
            PredictedPos.transform.localEulerAngles = new Vector3(float.Parse(colums[3]), float.Parse(colums[4]), float.Parse(colums[5]));
            PredictedPos.GetComponent<MeshRenderer>().material.color = new Color((float)0.0f, (float)0.0f, (float)1.0f);
            yield return new WaitForSeconds(0.5f);

            yield return null;
        }
    }

}
