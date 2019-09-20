using UnityEngine;
using System.Collections.Generic;

public class BFS
{
    class NodeRecord
    {
        public MapManager.NavPoint point;
        public int parentIndex;
        public int hScore;
        public NodeRecord(MapManager.NavPoint p, int parent)
        {
            point = p;
            parentIndex = parent;
            hScore = Mathf.Abs(p.row - targetPoint.row) + Mathf.Abs(p.col - targetPoint.col);
        }
    }
    readonly static int[] rowNeighbors = new int[] { -1, 1, 0, 0 };
    readonly static int[] colNeighbors = new int[] { 0, 0, -1, 1 };

    static MapManager.NavPoint targetPoint;
    static LinkedList<MapManager.NavPoint> listNavResult = new LinkedList<MapManager.NavPoint>();
    static Dictionary<int, bool> dicClosedNodes = new Dictionary<int, bool>();
    static List<NodeRecord> listOpenNodes = new List<NodeRecord>(MapManager.MaxRow * MapManager.MaxCol);

    public static LinkedList<MapManager.NavPoint> Navigation(MapManager.NavPoint start, MapManager.NavPoint target)
    {
        targetPoint = target;
        dicClosedNodes.Clear();
        listNavResult.Clear();
        listOpenNodes.Clear();
        listOpenNodes.Add(new NodeRecord(start, -10086));
        List<NodeRecord> listNeighbors = new List<NodeRecord>(4);
        int current = -1;
        while (++current < listOpenNodes.Count)
        {
            MapManager.NavPoint point = listOpenNodes[current].point;
            if (dicClosedNodes.ContainsKey(point.id)) continue;
            dicClosedNodes[point.id] = true;
            if (point == target)
            {
                do listNavResult.AddFirst(listOpenNodes[current].point);
                while ((current = listOpenNodes[current].parentIndex) > -1);
                return listNavResult;
            }
            if (point.type != 0) continue;

            listNeighbors.Clear();
            for (int i = 0, length = rowNeighbors.Length; i < length; ++i)
            {
                MapManager.NavPoint neighbor = MapManager.Instance.GetPoint(point.row + rowNeighbors[i], point.col + colNeighbors[i]);
                if (neighbor && !dicClosedNodes.ContainsKey(neighbor.id))
                    listNeighbors.Add(new NodeRecord(neighbor, current));
            }
            listNeighbors.Sort((a, b) => { return a.hScore - b.hScore; });

            for (int i = 0, length = listNeighbors.Count; i < length; ++i)
                listOpenNodes.Add(listNeighbors[i]);
        }
        return listNavResult;
    }
}
