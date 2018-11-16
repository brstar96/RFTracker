using UnityEngine;
using System.Collections;
using System.Collections.Generic; //list collection을 사용하기 위함

public class MeshGen : MonoBehaviour
{

    public List<Vector3> _Vertex = new List<Vector3>(); //버텍스들의 위치를 저장할 리스트 객체
    public List<int> _Tri = new List<int>(); //다른 인접 버텍스들의 인덱스를 기억하기 위한 객체
    public List<Vector2> _UV = new List<Vector2>(); //UV좌표 저장을 위한 vector2 변수
    private Mesh mesh;

    //Cell간의 Gap입니다. 5by5짜리 텍스쳐니까 간단히 1/5 = 0.2!
    private float _cellGap = 0.2f;
    //cell들의 Index를 저장합니다.
    public List<Vector2> _cell = new List<Vector2>();

    void Start()
    {
        mesh = GetComponent<MeshFilter>().mesh; //MeshFilter Component 를 가져옴

        var xPos = 0;
        var yPos = 0;
        var zPos = 0;



        //버텍스 포지션 지정하는 부분
        //float x = transform.position.x;
        //float y = transform.position.y;
        //float z = transform.position.z;

        //_Vertex.Add(new Vector3(x, y, z));
        //_Vertex.Add(new Vector3(x + 1, y, z));
        //_Vertex.Add(new Vector3(x + 1, y - 1, z));
        //_Vertex.Add(new Vector3(x, y - 1, z));

        //1번 폴리곤 
        //_Tri.Add(0);
        //_Tri.Add(1);
        //_Tri.Add(3);

        //2번 폴리곤 
        //_Tri.Add(1);
        //_Tri.Add(2);
        //_Tri.Add(3);

        //셀은 2개만 사용할겁니다. 0,0에 있는 검은색과 0,1에 있는 흰색입니다.
        //_cell.Add(new Vector2(0, 0));
        //_cell.Add(new Vector2(0, 1));

        //텍스쳐의 좌표를 UV에 넣습니다.
        //만약 List 인덱스 0번이라면 (0,0), (0.2,0), (0, 0.2), (0.2, 0.2)의 값을 가지겠죠?
        _UV.Add(new Vector2(_cellGap * _cell[1].x, _cellGap * _cell[1].y + _cellGap));
        _UV.Add(new Vector2(_cellGap * _cell[1].x + _cellGap, _cellGap * _cell[1].y + _cellGap));
        _UV.Add(new Vector2(_cellGap * _cell[1].x + _cellGap, _cellGap * _cell[1].y));
        _UV.Add(new Vector2(_cellGap * _cell[1].x, _cellGap * _cell[1].y));

        mesh.Clear(); //mesh를 청소
        mesh.vertices = _Vertex.ToArray(); //버텍스 데이터를 배열로 밀어넣음
        mesh.triangles = _Tri.ToArray(); //인접 버텍스 데이터를 배열로 밀어넣음
        //UV값을 넣습니다.
        mesh.uv = _UV.ToArray();
        mesh.RecalculateNormals();
    }
}