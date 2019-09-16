using UnityEngine;
using System.Collections.Generic;

public class BFS
{
    struct NodeRecord
    {
        public MapManager.NavPoint point;
        public int parentIndex;
    }

    static LinkedList<MapManager.NavPoint> listNavResult = new LinkedList<MapManager.NavPoint>();
    static Dictionary<MapManager.NavPoint, bool> dicClosedNodes = new Dictionary<MapManager.NavPoint, bool>();
    static List<NodeRecord> listOpenNodes = new List<NodeRecord>(MapManager.MaxRow * MapManager.MaxCol);

    public static LinkedList<MapManager.NavPoint> Navagation(MapManager.NavPoint start, MapManager.NavPoint target)
    {
        dicClosedNodes.Clear();
        listNavResult.Clear();
        listOpenNodes.Clear();
        listOpenNodes.Add(new NodeRecord() { point = start, parentIndex = -10086 });
        List<NodeRecord> listNeighbors = new List<NodeRecord>(4);
        int current = -1;
        while (++current < listOpenNodes.Count)
        {
            MapManager.NavPoint point = listOpenNodes[current].point;
            if (dicClosedNodes.ContainsKey(point)) continue;
            dicClosedNodes[point] = true;
            if (point == target)
            {
                while (current > -1)
                {
                    listNavResult.AddFirst(listOpenNodes[current].point);
                    current = listOpenNodes[current].parentIndex;
                }
                return listNavResult;
            }
            if (point.type != 0) continue;

            listNeighbors.Clear();
            MapManager.NavPoint neighbor = MapManager.Instance.GetPoint(point.row - 1, point.col);
            if (neighbor && !dicClosedNodes.ContainsKey(neighbor)) listNeighbors.Add(new NodeRecord() { point = neighbor, parentIndex = current });
            neighbor = MapManager.Instance.GetPoint(point.row + 1, point.col);
            if (neighbor && !dicClosedNodes.ContainsKey(neighbor)) listNeighbors.Add(new NodeRecord() { point = neighbor, parentIndex = current });
            neighbor = MapManager.Instance.GetPoint(point.row, point.col + 1);
            if (neighbor && !dicClosedNodes.ContainsKey(neighbor)) listNeighbors.Add(new NodeRecord() { point = neighbor, parentIndex = current });
            neighbor = MapManager.Instance.GetPoint(point.row, point.col - 1);
            if (neighbor && !dicClosedNodes.ContainsKey(neighbor)) listNeighbors.Add(new NodeRecord() { point = neighbor, parentIndex = current });
            listNeighbors.Sort((a, b) =>
            {
                return (a.point.position - target.position).sqrMagnitude.CompareTo((b.point.position - target.position).sqrMagnitude);
            });

            for (int i = 0, length = listNeighbors.Count; i < length; ++i)
                listOpenNodes.Add(listNeighbors[i]);
        }
        return listNavResult;
    }
}
