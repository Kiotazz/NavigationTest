using UnityEngine;
using System.Collections.Generic;

public static class BFS
{
    class NodeRecord
    {
        public MapManager.NavPoint point;
        public NodeRecord parentRecord;
        public int hScore;
        public NodeRecord(MapManager.NavPoint p, NodeRecord parent)
        {
            point = p;
            parentRecord = parent;
            hScore = Mathf.Abs(p.row - targetPoint.row) + Mathf.Abs(p.col - targetPoint.col);
        }
        public static implicit operator bool(NodeRecord record) { return record != null; }
    }
    readonly static int[] rowNeighbors = new int[] { -1, 1, 0, 0 };
    readonly static int[] colNeighbors = new int[] { 0, 0, -1, 1 };

    static MapManager.NavPoint targetPoint;
    static LinkedList<MapManager.NavPoint> listNavResult = new LinkedList<MapManager.NavPoint>();
    static Dictionary<int, bool> dicClosedNodes = new Dictionary<int, bool>();
    static LinkedList<NodeRecord> listOpenNodes = new LinkedList<NodeRecord>();

    public static LinkedList<MapManager.NavPoint> Navigation(MapManager.NavPoint start, MapManager.NavPoint target)
    {
        targetPoint = target;
        dicClosedNodes.Clear();
        listNavResult.Clear();
        listOpenNodes.Clear();
        listOpenNodes.AddLast(new NodeRecord(start, null));
        List<NodeRecord> listNeighbors = new List<NodeRecord>(4);
        while (listOpenNodes.Count > 0)
        {
            NodeRecord curRecord = listOpenNodes.First.Value;
            MapManager.NavPoint curPoint = curRecord.point;
            listOpenNodes.RemoveFirst();
            if (dicClosedNodes.ContainsKey(curPoint.id)) continue;
            dicClosedNodes[curPoint.id] = true;
            if (curPoint == target)
            {
                do listNavResult.AddFirst(curRecord.point);
                while (curRecord = curRecord.parentRecord);
                return listNavResult;
            }
            if (curPoint.type < 1) continue;

            listNeighbors.Clear();
            for (int i = 0, length = rowNeighbors.Length; i < length; ++i)
            {
                MapManager.NavPoint neighbor = MapManager.Instance.GetPoint(curPoint.row + rowNeighbors[i], curPoint.col + colNeighbors[i]);
                if (neighbor && !dicClosedNodes.ContainsKey(neighbor.id))
                    listNeighbors.Add(new NodeRecord(neighbor, curRecord));
            }
            listNeighbors.Sort((a, b) => { return a.hScore - b.hScore; });

            for (int i = 0, length = listNeighbors.Count; i < length; ++i)
                listOpenNodes.AddLast(listNeighbors[i]);
        }
        return listNavResult;
    }
}
