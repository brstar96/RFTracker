using UnityEngine;
using System.Collections.Generic;

public class TransformationGrid : MonoBehaviour {

	public Transform prefab;
    

	public int gridResolution = 10;

	Transform[] grid;
    
	List<Transformation> transformations;
    Matrix4x4 vm = Matrix4x4.identity;



	Matrix4x4 transformation;

	void Awake () {
		grid = new Transform[gridResolution * gridResolution * gridResolution];
		for (int i = 0, z = 0; z < gridResolution; z++) {
			for (int y = 0; y < gridResolution; y++) {
				for (int x = 0; x < gridResolution; x++, i++) {
					grid[i] = CreateGridPoint(x, y, z);//0,0,0
				}
			}
		}
		transformations = new List<Transformation>();
	}

    Transform CreateGridPoint(int x, int y, int z) {//0,0,0
        Transform point = Instantiate<Transform>(prefab);//sphere's transformaion
        point.localPosition = GetCoordinates(x, y, z);//0,0,0
        point.GetComponent<MeshRenderer>().material.color = new Color(
            (float)x / gridResolution,
            (float)y / gridResolution,
            (float)z / gridResolution
            //1.0f, 1.0f, 1.0f
        );
		return point;
	}


    

	Vector3 GetCoordinates (int x, int y, int z) {
		return new Vector3(
			x - (gridResolution - 1) * 0.5f,//-4.5
			y - (gridResolution - 1) * 0.5f, //-4.5
            z - (gridResolution - 1) * 0.5f//-4.5
        );
	}

	void Update () {
		UpdateTransformation();
		for (int i = 0, z = 0; z < gridResolution; z++) {
			for (int y = 0; y < gridResolution; y++) {
				for (int x = 0; x < gridResolution; x++, i++) {
					//grid[i].localPosition = TransformPoint(x, y, z);
				}
			}
		}
	}

	void UpdateTransformation () {
		GetComponents<Transformation>(transformations);
		if (transformations.Count > 0) {
			transformation = transformations[0].Matrix;
			for (int i = 1; i < transformations.Count; i++) {
				transformation = transformations[i].Matrix * transformation;
			}
		}
	}

	Vector3 TransformPoint (int x, int y, int z) {
		Vector3 coordinates = GetCoordinates(x, y, z);
		return transformation.MultiplyPoint(coordinates);
	}
}