using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace CoinGame
{
    public class CoinEater : MonoBehaviour
    {
        public int row { get; private set; }
        public int col { get; private set; }

        public int TarRow { get; private set; }
        public int TarCol { get; private set; }

        public void Init(int row, int col)
        {
            this.row = row;
            this.col = col;

            transform.position = new Vector3(col, 0.1f, row);
        }

        MapPoint FindNextPoint()
        {
            List<MapPoint> listNearstPoints = new List<MapPoint>();
            int max = Mathf.Max(Mathf.Max(MapManager.MaxRow - row, row), Mathf.Max(MapManager.MaxCol - col, col));
            for (int i = 0; i < max; i++)
            {
                for (int j = col - i, length = col + i; j < length; ++j)
                {
                    if (NavLibrary.GetPointStatus(row - i, j) > 0)
                        listNearstPoints.Add(new MapPoint(row - i, j, GetDistance(row - 1, j)));
                    if (NavLibrary.GetPointStatus(row + i, j) > 0)
                        listNearstPoints.Add(new MapPoint(row + i, j, GetDistance(row + 1, j)));
                }

                for (int j = row - i + 1, length = row + i - 1; j < length; ++j)
                {
                    if (NavLibrary.GetPointStatus(j, col - i) > 0)
                        listNearstPoints.Add(new MapPoint(j, col - i, GetDistance(j, col - i)));
                    if (NavLibrary.GetPointStatus(j, col + i) > 0)
                        listNearstPoints.Add(new MapPoint(j, col + i, GetDistance(j, col + i)));
                }

                if (listNearstPoints.Count > 7) break;
            }

            for (int i = 0, length = MapManager.Instance.listBigCoins.Count; i < length; ++i)
            {
                BigCoin coin = MapManager.Instance.listBigCoins[i];
                if (coin.Alive)
                    listNearstPoints.Add(new MapPoint(coin.row, coin.col, GetDistance(coin.row, coin.col)));
            }

            listNearstPoints.Sort((a, b) =>
            {
                float aScore = NavLibrary.GetPointStatus(a.row, b.col), bScore = NavLibrary.GetPointStatus(a.row, b.col);
                aScore -= a.value / MapManager.Instance.fDistanceWeightLv;
                bScore -= b.value / MapManager.Instance.fDistanceWeightLv;
                for (int i = 0, length = MapManager.Instance.listEaters.Count; i < length; ++i)
                {
                    CoinEater eater = MapManager.Instance.listEaters[i];
                    if (eater == this) continue;
                    if (eater.TarRow == a.row && eater.TarCol == a.col && eater.GetDistance(a.row, a.col) < a.value)
                        aScore -= 100;
                    if (eater.TarRow == b.row && eater.TarCol == b.col && eater.GetDistance(b.row, b.col) < b.value)
                        bScore -= 100;
                }

                int disToCenter = Mathf.Abs(MapManager.MaxRow / 2 - a.row) + Mathf.Abs(MapManager.MaxCol / 2 - a.col);
                aScore -= disToCenter / MapManager.MaxRow;
                disToCenter = Mathf.Abs(MapManager.MaxRow / 2 - b.row) + Mathf.Abs(MapManager.MaxCol / 2 - b.col);
                bScore -= disToCenter / MapManager.MaxRow;
                return aScore > bScore ? 1 : -1;
            });

            return listNearstPoints.Count > 0 ? listNearstPoints[0] : new MapPoint(-1, -1);
        }

        public int GetDistance(int tarRow, int tarCol)
        {
            return Mathf.Abs(tarRow - row) + Mathf.Abs(tarCol - col);
        }
    }
}
