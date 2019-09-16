﻿using System.Collections.Generic;
using UnityEngine;

public class MapManager : MonoBehaviour
{
    public static MapManager Instance;

    public const int MaxRow = 1000;
    public const int MaxCol = 1000;

    public class NavPoint
    {
        public int row;
        public int col;
        public int type;
        public Vector3 position;

        public static implicit operator bool(NavPoint point) { return point != null; }
    }

    public List<List<NavPoint>> mapData = new List<List<NavPoint>>();

    private void Start()
    {
        Instance = this;
        for (int i = 0; i < MaxRow; ++i)
        {
            List<NavPoint> col = new List<NavPoint>();
            mapData.Add(col);
            for (int j = 0; j < MaxCol; ++j)
                col.Add(new NavPoint() { row = i, col = j, position = new Vector3(j, 0, i), type = IsBrickCanPass(i, j) ? 0 : 1 });
        }

        GameObject objPrefab = Resources.Load<GameObject>("Prefabs/Brick");
        GameObject objBrick = Instantiate(objPrefab, transform);
        objBrick.name = "Ground";
        objBrick.transform.GetChild(0).gameObject.layer = LayerMask.NameToLayer("Brick");
        objBrick.transform.localScale = new Vector3(MaxCol, 1, MaxRow);
        objBrick.transform.position = new Vector3(MaxCol / 2 - 0.5f, 0, MaxRow / 2 - 0.5f);
        objBrick.GetComponentInChildren<Renderer>().material.mainTextureScale = new Vector2(MaxCol, MaxRow);

        GameObject objWallsRoot = new GameObject("WallsRoot");
        objWallsRoot.transform.SetParent(transform);
        for (int i = 0, lenRow = (int)Mathf.Ceil(MaxRow / RectMeshCreater.MaxRow); i < lenRow; ++i)
        {
            for (int j = 0, lenCol = (int)Mathf.Ceil(MaxCol / RectMeshCreater.MaxCol); j < lenCol; ++j)
            {
                RectMeshCreater.GenerateParams param = new RectMeshCreater.GenerateParams()
                {
                    row = RectMeshCreater.MaxRow,
                    col = RectMeshCreater.MaxCol,
                    startRow = i * RectMeshCreater.MaxRow,
                    startCol = j * RectMeshCreater.MaxCol,
                    materialPath = "Assets/Resources/Materials/WallMat.mat",
                    objName = "Wall_" + i + "_" + j,
                    parent = objWallsRoot.transform,
                    cbIsGenerateCell = (cellRow, cellCol) =>
                    {
                        NavPoint point = GetPoint(cellRow, cellCol);
                        return point && point.type != 0;
                    }
                };
                RectMeshCreater.GenerateRectObj(new Vector3(j * RectMeshCreater.MaxCol, 0, i * RectMeshCreater.MaxRow), param);
            }
        }

        GameObject objPlayer = Instantiate(Resources.Load<GameObject>("Prefabs/Myself"));
        objPlayer.transform.position = Vector3.zero;
    }

    public NavPoint GetPoint(int row, int col)
    {
        if (row < 0 || row >= MaxRow || col < 0 || col >= MaxCol) return null;
        return mapData[row][col];
    }

    public static bool IsBrickCanPass(int row, int col)
    {
        return row == 0 && col == 0 ? true : Random.Range(0, 10) > 0;
        //int total = row + col;
        //return total % 2 == 0 || total % 3 == 0 || total % 7 == 0;
    }
}