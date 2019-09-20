using UnityEngine;
using System.Collections.Generic;

public class A_Star
{
    class NodeRecord
    {
        public MapManager.NavPoint point;
        public NodeRecord parentRecord;
        public int hScore;
        public int gScore;
        public int fScore;
        public NodeRecord(MapManager.NavPoint p, NodeRecord parent)
        {
            point = p;
            fScore = (Mathf.Abs(p.row - curTarget.row) + Mathf.Abs(p.col - curTarget.col)) + (gScore = (parentRecord = parent) == null ? 0 : parent.gScore + 1);
        }
        public static implicit operator bool(NodeRecord record) { return record != null; }
    }

    readonly static int[] rowNeighbors = new int[] { -1, 1, 0, 0 };
    readonly static int[] colNeighbors = new int[] { 0, 0, -1, 1 };

    static LinkedList<MapManager.NavPoint> listNavResult = new LinkedList<MapManager.NavPoint>();
    static Dictionary<int, bool> dicClosedNodes = new Dictionary<int, bool>();
    static List<NodeRecord> listOpenNodes = new List<NodeRecord>(MapManager.MaxRow * MapManager.MaxCol);
    static Dictionary<int, NodeRecord> dicOpenNodes = new Dictionary<int, NodeRecord>();

    static MapManager.NavPoint curTarget;

    public static LinkedList<MapManager.NavPoint> Navigation(MapManager.NavPoint start, MapManager.NavPoint target)
    {
        curTarget = target;
        dicClosedNodes.Clear();
        listNavResult.Clear();
        listOpenNodes.Clear();
        dicOpenNodes.Clear();
        NodeRecord record = new NodeRecord(start, null);
        listOpenNodes.Add(record);
        dicOpenNodes[start.id] = record;
        while (listOpenNodes.Count > 0)
        {
            NodeRecord curRecord = listOpenNodes[listOpenNodes.Count - 1];
            MapManager.NavPoint curPoint = curRecord.point;
            listOpenNodes.RemoveAt(listOpenNodes.Count - 1);
            dicOpenNodes.Remove(curPoint.id);
            if (dicClosedNodes.ContainsKey(curPoint.id)) continue;
            dicClosedNodes[curPoint.id] = true;

            if (curPoint == target)
            {
                do listNavResult.AddFirst(curRecord.point);
                while (curRecord = curRecord.parentRecord);
                return listNavResult;
            }
            if (curPoint.type != 0) continue;

            for (int i = 0, length = rowNeighbors.Length; i < length; ++i)
                AddChildPoint(MapManager.Instance.GetPoint(curPoint.row + rowNeighbors[i], curPoint.col + colNeighbors[i]), curRecord);
            listOpenNodes.Sort((a, b) => { return b.fScore - a.fScore; });
        }
        return listNavResult;
    }

    static void AddChildPoint(MapManager.NavPoint point, NodeRecord curRecord)
    {
        if (!point || dicClosedNodes.ContainsKey(point.id)) return;
        if (dicOpenNodes.ContainsKey(point.id))
        {
            NodeRecord record = dicOpenNodes[point.id];
            if (curRecord.gScore < record.gScore)
            {
                record.parentRecord = curRecord;
                record.fScore = record.hScore + (record.gScore = curRecord.gScore + 1);
            }
            return;
        }
        NodeRecord newRecord = new NodeRecord(point, curRecord);
        listOpenNodes.Add(newRecord);
        dicOpenNodes[point.id] = newRecord;
    }
}
