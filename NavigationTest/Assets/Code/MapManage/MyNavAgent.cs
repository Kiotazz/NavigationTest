using System.Collections.Generic;
using UnityEngine;
using DG.Tweening;

public class MyNavAgent : MonoBehaviour
{
    public static MyNavAgent Instance;

    public enum NavType
    {
        DFS,
        BFS,
        A_Star
    }
    public NavType navType = NavType.DFS;
    public MapManager.NavPoint CurrentPoint { get; private set; }
    public bool IsMoving { get; private set; }
    LinkedList<MapManager.NavPoint> moveRode;

    Camera cameraWorld;
    GameObject objSign;

    void Start()
    {
        Instance = this;
        IsMoving = false;
        cameraWorld = FindObjectOfType<Camera>();
        CurrentPoint = MapManager.Instance.GetPoint(0, 0);

        objSign = Instantiate(Resources.Load<GameObject>("Prefabs/Sign"));
        objSign.transform.parent = MapManager.Instance.transform;
        objSign.SetActive(false);
    }

    private void Update()
    {
        if (Input.GetMouseButtonDown(0))
        {
            RaycastHit hit;
            if (Physics.Raycast(cameraWorld.ScreenPointToRay(Input.mousePosition), out hit, 100, 1 << LayerMask.NameToLayer("Brick")))
            {
                //string[] name = hit.collider.transform.parent.name.Split('_');
                //SetTarget(MapManager.Instance.GetPoint(int.Parse(name[0]), int.Parse(name[1])));
                SetTarget(MapManager.Instance.GetPoint((int)Mathf.Ceil(hit.point.z - 0.5f), (int)Mathf.Ceil(hit.point.x - 0.5f)));
            }
        }
    }

    public void SetTarget(MapManager.NavPoint point)
    {
        moveRode = null;
        objSign.SetActive(true);
        objSign.transform.position = point.position;
        System.Diagnostics.Stopwatch stopWatch = new System.Diagnostics.Stopwatch();
        stopWatch.Start();
        switch (navType)
        {
            case NavType.DFS:
                moveRode = DFS.Navagation(CurrentPoint, point);
                break;
            case NavType.BFS:
                moveRode = BFS.Navagation(CurrentPoint, point);
                break;
            case NavType.A_Star:
                moveRode = A_Star.Navagation(CurrentPoint, point);
                break;
            default:
                moveRode = null;
                break;
        }
        stopWatch.Stop();
        Debug.Log("Cost Time: " + stopWatch.Elapsed.TotalMilliseconds);
        if (!IsMoving)
            MoveToNextPoint();
    }

    void MoveToNextPoint()
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
            MoveToNextPoint();
            return;
        }
        Tweener tweener = transform.DOMove(point.position, 0.1f);
        tweener.SetEase(Ease.InOutQuad);
        tweener.onComplete = () => { IsMoving = false; MoveToNextPoint(); };
        IsMoving = true;
        CurrentPoint = point;
    }
}
