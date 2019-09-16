using UnityEngine;
using System.Collections.Generic;

public class DFS
{
    static MapManager.NavPoint curTarget;
    static LinkedList<MapManager.NavPoint> listNavResult = new LinkedList<MapManager.NavPoint>();
    static Dictionary<MapManager.NavPoint, bool> dicClosedNodes = new Dictionary<MapManager.NavPoint, bool>();

    public static LinkedList<MapManager.NavPoint> Navagation(MapManager.NavPoint start, MapManager.NavPoint target)
    {
        curTarget = target;
        dicClosedNodes.Clear();
        listNavResult.Clear();
        FindNextPoint(start);
        return listNavResult;
    }

    static bool FindNextPoint(MapManager.NavPoint point)
    {
        dicClosedNodes[point] = true;
        if (point == curTarget)
        {
            listNavResult.AddFirst(point);
            return true;
        }
        if (point.type != 0) return false;

        List<MapManager.NavPoint> listNeighbors = new List<MapManager.NavPoint>(4);
        MapManager.NavPoint nPoint = MapManager.Instance.GetPoint(point.row - 1, point.col);
        if (nPoint) listNeighbors.Add(nPoint);
        nPoint = MapManager.Instance.GetPoint(point.row + 1, point.col);
        if (nPoint) listNeighbors.Add(nPoint);
        nPoint = MapManager.Instance.GetPoint(point.row, point.col + 1);
        if (nPoint) listNeighbors.Add(nPoint);
        nPoint = MapManager.Instance.GetPoint(point.row, point.col - 1);
        if (nPoint) listNeighbors.Add(nPoint);
        listNeighbors.Sort((a, b) =>
        {
            return (a.position - curTarget.position).sqrMagnitude.CompareTo((b.position - curTarget.position).sqrMagnitude);
        });

        for (int i = 0, length = listNeighbors.Count; i < length; ++i)
        {
            if (!dicClosedNodes.ContainsKey(listNeighbors[i]) && FindNextPoint(listNeighbors[i]))
            {
                listNavResult.AddFirst(point);
                return true;
            }
        }
        return false;
    }
}
