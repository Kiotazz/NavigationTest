using System.Collections.Generic;
using UnityEngine;
using UnityEngine.EventSystems;
using DG.Tweening;

public class MyNavAgent : MonoBehaviour
{
    public static MyNavAgent Instance;

    public NavLibrary.Algorithm navType = NavLibrary.Algorithm.DFS;
    public float fMoveCost = 0.1f;

    public bool IsMoving { get; private set; }

    Camera cameraWorld;
    GameObject objSign;

    void Start()
    {
        Instance = this;
        IsMoving = false;
        cameraWorld = FindObjectOfType<Camera>();

        objSign = Instantiate(Resources.Load<GameObject>("Prefabs/Sign"));
        objSign.name = "Sign";
        objSign.transform.parent = MapManager.Instance.transform;
        objSign.SetActive(false);

        if (MapManager.Instance.IsNative)
            curRow = curCol = 0;
        else
            currentPoint = MapManager.Instance.GetPoint(0, 0);
    }

    private void Update()
    {
        if (Input.GetMouseButtonDown(0) && !EventSystem.current.IsPointerOverGameObject())
        {
            RaycastHit hit;
            if (Physics.Raycast(cameraWorld.ScreenPointToRay(Input.mousePosition), out hit, 100, 1 << LayerMask.NameToLayer("Brick")))
                SetTarget(Mathf.CeilToInt(hit.point.z - 0.5f), Mathf.CeilToInt(hit.point.x - 0.5f));
        }
    }

    public void SetTarget(int row, int col)
    {
        if (MapManager.Instance.IsNative)
            SetTarget_Native(row, col);
        else
            SetTarget_Local(MapManager.Instance.GetPoint(row, col));
    }


    int curRow = 0;
    int curCol = 0;
    int curStep = 0;

    public void SetTarget_Native(int row, int col)
    {
        moveRode = null;
        objSign.SetActive(true);
        objSign.transform.position = new Vector3(col, 0, row);
        System.Diagnostics.Stopwatch stopWatch = new System.Diagnostics.Stopwatch();
        stopWatch.Start();
        int roadLength = NavLibrary.Navigation(curRow, curCol, row, col, navType);
        stopWatch.Stop();
        MainView.Instance.RefreshInfo(row, col, stopWatch.Elapsed.TotalMilliseconds);
        Debug.Log("Cost Time: " + stopWatch.Elapsed.TotalMilliseconds);
        Debug.Log("Road Length:" + roadLength);
        curStep = -1;
        if (!IsMoving)
            MoveToNextPoint_Native();
    }

    void MoveToNextPoint_Native()
    {
        if (IsMoving) return;
        if (!NavLibrary.GetPoint(++curStep, ref curRow, ref curCol))
        {
            IsMoving = false;
            objSign.SetActive(false);
            return;
        }
        Vector3 position = new Vector3(curCol, 0, curRow);
        if (position == transform.position)
        {
            MoveToNextPoint_Native();
            return;
        }
        Tweener tweener = transform.DOMove(position, fMoveCost);
        tweener.SetEase(Ease.InOutQuad);
        tweener.onComplete = () => { IsMoving = false; MoveToNextPoint_Native(); };
        IsMoving = true;
    }


    MapManager.NavPoint currentPoint;
    LinkedList<MapManager.NavPoint> moveRode;

    public void SetTarget_Local(MapManager.NavPoint point)
    {
        moveRode = null;
        objSign.SetActive(true);
        objSign.transform.position = point.position;
        System.Diagnostics.Stopwatch stopWatch = new System.Diagnostics.Stopwatch();
        stopWatch.Start();
        switch (navType)
        {
            case NavLibrary.Algorithm.DFS:
                moveRode = DFS.Navigation(currentPoint, point);
                break;
            case NavLibrary.Algorithm.BFS:
                moveRode = BFS.Navigation(currentPoint, point);
                break;
            case NavLibrary.Algorithm.A_Star:
                moveRode = A_Star.Navigation(currentPoint, point);
                break;
            default:
                moveRode = null;
                break;
        }
        stopWatch.Stop();
        MainView.Instance.RefreshInfo(point.row, point.col, stopWatch.Elapsed.TotalMilliseconds);
        Debug.Log("Cost Time: " + stopWatch.Elapsed.TotalMilliseconds);
        if (!IsMoving)
            MoveToNextPoint_Local();
    }

    void MoveToNextPoint_Local()
    {
        if (IsMoving) return;
        if (moveRode == null || moveRode.Count < 1)
        {
            IsMoving = false;
            objSign.SetActive(false);
            return;
        }
        MapManager.NavPoint point = moveRode.First.Value;
        moveRode.RemoveFirst();
        if (point.position == transform.position || point.type != 0)
        {
            MoveToNextPoint_Local();
            return;
        }
        Tweener tweener = transform.DOMove(point.position, fMoveCost);
        tweener.SetEase(Ease.InOutQuad);
        tweener.onComplete = () => { IsMoving = false; MoveToNextPoint_Local(); };
        IsMoving = true;
        currentPoint = point;
    }
}
