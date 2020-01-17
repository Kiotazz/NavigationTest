using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Events;

namespace CoinGame
{
    public class MapManager : MonoBehaviour
    {
        public static MapManager Instance;


        public int nEaterNum = 5;
        public float fRefreshInterval = 3;
        public int nBigCoinNum = 2;
        public int nBigCoinValue = 3;
        public float fBigCoinInterval = 10;

        public float fDistanceWeightLv = 5;

        public UnityEvent OnMapRefresh = new UnityEvent();
        public MapPointEvent OnBigCoinRefresh = new MapPointEvent();
        public MapPointEvent OnBigCoinEat = new MapPointEvent();
        public const int MaxRow = 100;
        public const int MaxCol = 100;

        float fTimeCounter = 0;
        MeshFilter mfCoins;

        public List<CoinEater> listEaters = new List<CoinEater>();
        public List<BigCoin> listBigCoins = new List<BigCoin>();

        private void Start()
        {
            Instance = this;
            NavLibrary.InitMap(MaxRow, MaxCol, GeneratePointStatus);

            GameObject objPrefab = Resources.Load<GameObject>("Prefabs/Brick");
            GameObject objBrick = Instantiate(objPrefab, transform);
            objBrick.name = "Ground";
            objBrick.transform.GetChild(0).gameObject.layer = LayerMask.NameToLayer("Brick");
            objBrick.transform.localScale = new Vector3(MaxCol, 1, MaxRow);
            objBrick.transform.position = new Vector3(MaxCol / 2 - 0.5f, 0, MaxRow / 2 - 0.5f);
            objBrick.GetComponentInChildren<Renderer>().material.mainTextureScale = new Vector2(MaxCol, MaxRow);

            GameObject objWallsRoot = new GameObject("WallsRoot");
            objWallsRoot.transform.SetParent(transform);
            for (int i = 0, lenRow = Mathf.CeilToInt(MaxRow / RectMeshCreater.MaxRow); i < lenRow; ++i)
            {
                for (int j = 0, lenCol = Mathf.CeilToInt(MaxCol / RectMeshCreater.MaxCol); j < lenCol; ++j)
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
                        mfOperate = mfCoins,
                        cbIsGenerateCell = (cellRow, cellCol) =>
                        {
                            return NavLibrary.GetPointStatus(cellRow, cellCol) == 0;
                        }
                    };
                    mfCoins = RectMeshCreater.GenerateRectObj(new Vector3(j * RectMeshCreater.MaxCol, 0.01f, i * RectMeshCreater.MaxRow), param);
                }
            }

            GameObject objPlayer = Instantiate(Resources.Load<GameObject>("Prefabs/Myself"));
            objPlayer.name = "Myself";
            objPlayer.transform.position = Vector3.zero;
        }

        public int GetPointValue(int row, int col)
        {
            return NavLibrary.GetPointStatus(row, col);
        }

        int GeneratePointStatus(int row, int col)
        {
            return Random.Range(1, 6) == 1 ? 2 : 1;
        }

        private void Update()
        {
            for (int i = listBigCoins.Count; i < nBigCoinNum; ++i)
                listBigCoins[i].DoUpdate();
            if ((fTimeCounter += Time.deltaTime) > fRefreshInterval)
            {
                fTimeCounter = 0;
                RefreshMap();
            }
        }

        void RefreshMap()
        {
            if (nEaterNum != listEaters.Count)
            {
                if (listEaters.Count > nBigCoinNum)
                {
                    for (int i = nBigCoinNum; i < listEaters.Count; ++i)
                    {
                        Destroy(listEaters[i].gameObject);
                        listEaters.RemoveAt(i);
                    }
                }
                else
                {
                    for (int i = listBigCoins.Count; i < nBigCoinNum; ++i)
                    {
                        BigCoin coin = new BigCoin();
                        coin.Init();
                        listBigCoins.Add(coin);
                    }
                }
            }

            if (nBigCoinNum != listBigCoins.Count)
            {
                if (listBigCoins.Count > nBigCoinNum)
                {
                    for (int i = nBigCoinNum; i < listBigCoins.Count; ++i)
                    {
                        if (listBigCoins[i].Alive)
                            NavLibrary.SetPointStatus(listBigCoins[i].row, listBigCoins[i].col, 1);
                        listBigCoins.RemoveAt(i);
                    }
                }
                else
                {
                    GameObject objPrefab = Resources.Load<GameObject>("Prefabs/CoinEater");
                    for (int i = listBigCoins.Count; i < nBigCoinNum; ++i)
                    {
                        GameObject objEater = Instantiate(objPrefab, transform);
                        int row, col;
                        do
                        {
                            row = Random.Range(0, MaxRow + 1);
                            col = Random.Range(0, MaxCol + 1);
                        } while (!IsPointClear(row, col));
                        CoinEater eater = objPrefab.GetComponent<CoinEater>();
                        eater.Init(row, col);
                        listEaters.Add(eater);
                    }
                }
            }

            for (int i = 0; i < MaxRow; ++i)
                for (int j = 0; j < MaxCol; ++j)
                    if (GetPointValue(i, j) != nBigCoinValue)
                        NavLibrary.SetPointStatus(i, j, GeneratePointStatus(i, j));

            OnMapRefresh.Invoke();
        }

        public bool IsPointClear(int row, int col)
        {
            for (int i = 0; i < listEaters.Count; i++)
                if (listEaters[i].row == row && listEaters[i].col == col)
                    return false;
            return GetPointValue(row, col) != nBigCoinValue;
        }
    }

    public struct MapPoint
    {
        public int row;
        public int col;
        public int value;

        public MapPoint(int r, int c) { row = r; col = c; value = 0; }
        public MapPoint(int r, int c, int v) { row = r; col = c; value = v; }
    }

    public class MapPointEvent : UnityEvent<MapPoint> { }
}
