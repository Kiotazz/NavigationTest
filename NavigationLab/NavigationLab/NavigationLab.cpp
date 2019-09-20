// NavigationLab.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "NavigationLab.h"
using namespace std;

int maxRow = 1000;
int maxCol = 1000;

int rowNeighbors[]{ -1, 1, 0, 0 };
int colNeighbors[]{ 0, 0, -1, 1 };

map<int, map<int, bool>> mapObstacles;

int tarRow = 0;
int tarCol = 0;
vector<int> listNavResult;
map<int, map<int, bool>> mapClosedNodes;

inline bool IsTargetPoint(int row, int col) { return row == tarRow && col == tarCol; }

void ClosePoint(int row, int col)
{
	if (mapClosedNodes.find(row) == mapClosedNodes.end())
		mapClosedNodes[row] = map<int, bool>();
	mapClosedNodes[row][col] = true;
}

bool IsPointClosed(int row, int col)
{
	if (mapClosedNodes.find(row) == mapClosedNodes.end())
		return false;
	map<int, bool> colData = mapClosedNodes[row];
	return colData.find(col) != colData.end();
}

inline bool IsPointValid(int row, int col)
{
	return row > -1 && row < maxRow && col > -1 && col < maxCol;
}

bool IsPointCanPass(int row, int col)
{
	if (!IsPointValid(row, col)) return false;
	if (mapObstacles.find(row) == mapObstacles.end())
		return true;
	map<int, bool> colData = mapObstacles[row];
	return colData.find(col) == colData.end();
}

inline int GetPointHScore(int row, int col)
{
	return abs(row - tarRow) + abs(col - tarCol);
}


struct PointRecord_DFS
{
	int row;
	int col;
	int hScore;
	PointRecord_DFS(int r, int c)
	{
		row = r;
		col = c;
		hScore = GetPointHScore(r, c);
	}
	bool operator <(const PointRecord_DFS &other) const
	{
		return hScore < other.hScore;
	}
};

EX(void) InitMap(int row, int col)
{
	mapObstacles.clear();
	maxRow = row;
	maxCol = col;
	listNavResult.resize((row + col) * 3);
}

EX(void) SetPointObstacle(int row, int col)
{
	if (mapObstacles.find(row) == mapObstacles.end())
		mapObstacles[row] = map<int, bool>();
	mapObstacles[row][col] = true;
}

//DFS
bool FindNextPoint(int row, int col)
{
	ClosePoint(row, col);
	if (IsTargetPoint(row, col))
	{
		if (IsPointCanPass(row, col))
		{
			listNavResult.push_back(row);
			listNavResult.push_back(col);
		}
		return true;
	}
	if (!IsPointCanPass(row, col)) return false;

	set<PointRecord_DFS> setPoints;
	for (int i = 0; i < 4; ++i)
		if (IsPointValid(row + rowNeighbors[i], col + colNeighbors[i]))
			setPoints.insert(PointRecord_DFS(row + rowNeighbors[i], col + colNeighbors[i]));

	for (set<PointRecord_DFS>::iterator it = setPoints.begin(); it != setPoints.end(); ++it)
	{
		if (!IsPointClosed(it->row, it->col) && FindNextPoint(it->row, it->col))
		{
			listNavResult.push_back(it->row);
			listNavResult.push_back(it->col);
			return true;
		}
	}
	return false;
}

EX(int*) Navigation_DFS(int startRow, int startCol, int targetRow, int targetCol, int* size)
{
	tarRow = targetRow;
	tarCol = targetCol;
	mapClosedNodes.clear();
	listNavResult.clear();
	FindNextPoint(startRow, startCol);
	*size = listNavResult.size() / 2;
	return &listNavResult[0];
}


EX(int) TestMethod()
{
	return 10086;
}


//BFS
//struct NodeRecord
//{
//	public MapManager.NavPoint point;
//	public int parentIndex;
//}
//
//static LinkedList<MapManager.NavPoint> listNavResult = new LinkedList<MapManager.NavPoint>();
//static List<NodeRecord> listOpenNodes = new List<NodeRecord>(MapManager.MaxRow * MapManager.MaxCol);
//
//public static LinkedList<MapManager.NavPoint> Navigation(MapManager.NavPoint start, MapManager.NavPoint target)
//{
//	dicClosedNodes.Clear();
//	listNavResult.Clear();
//	listOpenNodes.Clear();
//	listOpenNodes.Add(new NodeRecord(){ point = start, parentIndex = -10086 });
//	List<NodeRecord> listNeighbors = new List<NodeRecord>(4);
//	int current = -1;
//	while (++current < listOpenNodes.Count)
//	{
//		MapManager.NavPoint point = listOpenNodes[current].point;
//		if (dicClosedNodes.ContainsKey(point)) continue;
//		dicClosedNodes[point] = true;
//		if (point == target)
//		{
//			while (current > -1)
//			{
//				listNavResult.AddFirst(listOpenNodes[current].point);
//				current = listOpenNodes[current].parentIndex;
//			}
//			return listNavResult;
//		}
//		if (point.type != 0) continue;
//
//		listNeighbors.Clear();
//		for (int i = 0, length = rowNeighbors.Length; i < length; ++i)
//		{
//			MapManager.NavPoint neighbor = MapManager.Instance.GetPoint(point.row + rowNeighbors[i], point.col + colNeighbors[i]);
//			if (neighbor && !dicClosedNodes.ContainsKey(neighbor))
//				listNeighbors.Add(new NodeRecord(){ point = neighbor, parentIndex = current });
//		}
//		listNeighbors.Sort((a, b) = >
//		{
//			return Mathf.Abs(a.point.row - target.row) + Mathf.Abs(a.point.col - target.col) - Mathf.Abs(b.point.row - target.row) - Mathf.Abs(b.point.col - target.col);
//		});
//
//		for (int i = 0, length = listNeighbors.Count; i < length; ++i)
//			listOpenNodes.Add(listNeighbors[i]);
//	}
//	return listNavResult;
//}



//A_Star
//class NodeRecord
//{
//	public MapManager.NavPoint point;
//	public NodeRecord parentRecord;
//	public int hScore;
//	public int gScore;
//	public int fScore;
//}
//
//static LinkedList<MapManager.NavPoint> listNavResult = new LinkedList<MapManager.NavPoint>();
//static Dictionary<MapManager.NavPoint, bool> dicClosedNodes = new Dictionary<MapManager.NavPoint, bool>();
//static List<NodeRecord> listOpenNodes = new List<NodeRecord>(MapManager.MaxRow * MapManager.MaxCol);
//
//static MapManager.NavPoint curTarget;
//
//public static LinkedList<MapManager.NavPoint> Navigation(MapManager.NavPoint start, MapManager.NavPoint target)
//{
//	curTarget = target;
//	dicClosedNodes.Clear();
//	listNavResult.Clear();
//	listOpenNodes.Clear();
//	int hScore = GetPointHScore(start);
//	listOpenNodes.Add(new NodeRecord(){ point = start, parentRecord = null, gScore = 0, hScore = hScore, fScore = hScore });
//	while (listOpenNodes.Count > 0)
//	{
//		NodeRecord curRecord = listOpenNodes[listOpenNodes.Count - 1];
//		MapManager.NavPoint curPoint = curRecord.point;
//		listOpenNodes.RemoveAt(listOpenNodes.Count - 1);
//		if (dicClosedNodes.ContainsKey(curPoint)) continue;
//		dicClosedNodes[curPoint] = true;
//
//		if (curPoint == target)
//		{
//			while (curRecord != null)
//			{
//				listNavResult.AddFirst(curRecord.point);
//				curRecord = curRecord.parentRecord;
//			}
//			return listNavResult;
//		}
//		if (curPoint.type != 0) continue;
//
//		for (int i = 0, length = rowNeighbors.Length; i < length; ++i)
//			AddChildPoint(MapManager.Instance.GetPoint(curPoint.row + rowNeighbors[i], curPoint.col + colNeighbors[i]), curRecord);
//		listOpenNodes.Sort((a, b) = > { return b.fScore.CompareTo(a.fScore); });
//	}
//	return listNavResult;
//}
//
//static void AddChildPoint(MapManager.NavPoint point, NodeRecord curRecord)
//{
//	if (!point || dicClosedNodes.ContainsKey(point)) return;
//	for (int i = 0, length = listOpenNodes.Count; i < length; ++i)
//	{
//		NodeRecord record = listOpenNodes[i];
//		if (record.point == point)
//		{
//			if (curRecord.gScore < record.gScore)
//			{
//				record.parentRecord = curRecord;
//				record.gScore = curRecord.gScore + 1;
//				record.fScore = record.hScore + record.gScore;
//			}
//			return;
//		}
//	}
//	int hScore = GetPointHScore(point);
//	listOpenNodes.Add(new NodeRecord()
//		{
//			point = point,
//			parentRecord = curRecord,
//			hScore = hScore,
//			gScore = curRecord.gScore + 1,
//			fScore = hScore + curRecord.gScore + 1
//		});
//}
//
//static int GetPointHScore(MapManager.NavPoint point)
//{
//	return Mathf.Abs(point.row - curTarget.row) + Mathf.Abs(point.col - curTarget.col);
//}
