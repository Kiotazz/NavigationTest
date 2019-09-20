using UnityEngine;
using System.Collections.Generic;

public static class DFS
{
    readonly static int[] rowNeighbors = new int[] { -1, 1, 0, 0 };
    readonly static int[] colNeighbors = new int[] { 0, 0, -1, 1 };

    static MapManager.NavPoint curTarget;
    static LinkedList<MapManager.NavPoint> listNavResult = new LinkedList<MapManager.NavPoint>();
    static Dictionary<int, bool> dicClosedNodes = new Dictionary<int, bool>();

    public static LinkedList<MapManager.NavPoint> Navigation(MapManager.NavPoint start, MapManager.NavPoint target)
    {
        curTarget = target;
        dicClosedNodes.Clear();
        listNavResult.Clear();
        FindNextPoint(start);
        return listNavResult;
    }

    static bool FindNextPoint(MapManager.NavPoint point)
    {
        dicClosedNodes[point.id] = true;
        if (point == curTarget)
        {
            listNavResult.AddFirst(point);
            return true;
        }
        if (point.type != 0) return false;

        List<MapManager.NavPoint> listNeighbors = new List<MapManager.NavPoint>(4);
        for (int i = 0, length = rowNeighbors.Length; i < length; ++i)
        {
            MapManager.NavPoint neighbor = MapManager.Instance.GetPoint(point.row + rowNeighbors[i], point.col + colNeighbors[i]);
            if (neighbor) listNeighbors.Add(neighbor);
        }
        listNeighbors.Sort((a, b) =>
        {
            return Mathf.Abs(a.row - curTarget.row) + Mathf.Abs(a.col - curTarget.col) - Mathf.Abs(b.row - curTarget.row) - Mathf.Abs(b.col - curTarget.col);
        });

        for (int i = 0, length = listNeighbors.Count; i < length; ++i)
        {
            if (!dicClosedNodes.ContainsKey(listNeighbors[i].id) && FindNextPoint(listNeighbors[i]))
            {
                listNavResult.AddFirst(point);
                return true;
            }
        }
        return false;
    }
}
