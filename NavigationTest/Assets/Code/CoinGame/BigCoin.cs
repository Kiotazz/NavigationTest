using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace CoinGame
{
    public class BigCoin
    {
        public int row { get; private set; }
        public int col { get; private set; }

        float fTimeCounter = 0;

        public bool Alive
        {
            get { return MapManager.Instance.GetPointValue(row, col) == MapManager.Instance.nBigCoinValue; }
        }

        // Use this for initialization
        public void Init()
        {
            GenerateCoin();
            MapManager.Instance.OnBigCoinEat.AddListener(OnBigCoinEat);
        }

        // Update is called once per frame
        public void DoUpdate()
        {
            if ((fTimeCounter += Time.deltaTime) > MapManager.Instance.fBigCoinInterval && !Alive)
                GenerateCoin();
        }

        void GenerateCoin()
        {
            fTimeCounter = 0;
            do
            {
                row = Random.Range(0, MapManager.MaxRow + 1);
                col = Random.Range(0, MapManager.MaxCol + 1);
            } while (!MapManager.Instance.IsPointClear(row, col));

            NavLibrary.SetPointStatus(row, col, MapManager.Instance.nBigCoinValue);
            MapManager.Instance.OnBigCoinRefresh.Invoke(new MapPoint(row, col));
        }

        void OnBigCoinEat(MapPoint point)
        {
            if (point.row != row || point.col != col) return;

        }
    }
}
