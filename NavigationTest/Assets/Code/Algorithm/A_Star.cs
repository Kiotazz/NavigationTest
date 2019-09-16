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
    }

    readonly static int[] rowNeighbors = new int[] { -1, 1, 0, 0 };
    readonly static int[] colNeighbors = new int[] { 0, 0, -1, 1 };

    static LinkedList<MapManager.NavPoint> listNavResult = new LinkedList<MapManager.NavPoint>();
    static Dictionary<MapManager.NavPoint, bool> dicClosedNodes = new Dictionary<MapManager.NavPoint, bool>();
    static List<NodeRecord> listOpenNodes = new List<NodeRecord>(MapManager.MaxRow * MapManager.MaxCol);

    static MapManager.NavPoint curTarget;

    public static LinkedList<MapManager.NavPoint> Navagation(MapManager.NavPoint start, MapManager.NavPoint target)
    {
        curTarget = target;
        dicClosedNodes.Clear();
        listNavResult.Clear();
        listOpenNodes.Clear();
        int hScore = GetPointHScore(start);
        listOpenNodes.Add(new NodeRecord() { point = start, parentRecord = null, gScore = 0, hScore = hScore, fScore = hScore });
        while (listOpenNodes.Count > 0)
        {
            NodeRecord curRecord = listOpenNodes[listOpenNodes.Count - 1];
            MapManager.NavPoint curPoint = curRecord.point;
            listOpenNodes.RemoveAt(listOpenNodes.Count - 1);
            if (dicClosedNodes.ContainsKey(curPoint)) continue;
            dicClosedNodes[curPoint] = true;

            if (curPoint == target)
            {
                while (curRecord != null)
                {
                    listNavResult.AddFirst(curRecord.point);
                    curRecord = curRecord.parentRecord;
                }
                return listNavResult;
            }
            if (curPoint.type != 0) continue;

            for (int i = 0, length = rowNeighbors.Length; i < length; ++i)
                AddNeighbor(MapManager.Instance.GetPoint(curPoint.row + rowNeighbors[i], curPoint.col + colNeighbors[i]), curRecord);
            listOpenNodes.Sort((a, b) => { return b.fScore.CompareTo(a.fScore); });
        }
        return listNavResult;
    }

    static void AddNeighbor(MapManager.NavPoint neighbor, NodeRecord curRecord)
    {
        if (!neighbor || dicClosedNodes.ContainsKey(neighbor)) return;
        for (int i = 0, length = listOpenNodes.Count; i < length; ++i)
        {
            NodeRecord record = listOpenNodes[i];
            if (record.point == neighbor)
            {
                if (curRecord.gScore < record.gScore)
                {
                    record.parentRecord = curRecord;
                    record.gScore = curRecord.gScore + 1;
                    record.fScore = record.hScore + record.gScore;
                }
                return;
            }
        }
        int hScore = GetPointHScore(neighbor);
        listOpenNodes.Add(new NodeRecord()
        {
            point = neighbor,
            parentRecord = curRecord,
            hScore = hScore,
            gScore = curRecord.gScore + 1,
            fScore = hScore + curRecord.gScore + 1
        });
    }

    static int GetPointHScore(MapManager.NavPoint point)
    {
        return Mathf.Abs(point.row - curTarget.row) + Mathf.Abs(point.col - curTarget.col);
    }
}
