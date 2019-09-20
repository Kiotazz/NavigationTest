// NavigationLab.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "NavigationLab.h"
#include "MinHeap.h"
using namespace std;

int maxRow = 1000;
int maxCol = 1000;

int rowNeighbors[]{ -1, 1, 0, 0 };
int colNeighbors[]{ 0, 0, -1, 1 };

map<int, bool> mapObstacles;

int tarRow = 0;
int tarCol = 0;
vector<int> listNavResult;
map<int, bool> mapClosedNodes;

inline bool IsTargetPoint(const int row, const int col) { return row == tarRow && col == tarCol; }
inline int GetPointID(const int row, const int col) { return row * maxCol + col; }
inline void ClosePoint(const int row, const int col) { mapClosedNodes[GetPointID(row, col)] = true; }
inline bool IsPointClosed(const int row, const int col) { return mapClosedNodes.find(GetPointID(row, col)) != mapClosedNodes.end(); }
inline bool IsPointValid(const int row, const int col) { return row > -1 && row < maxRow && col > -1 && col < maxCol; }
inline bool IsPointValidAndNotClosed(const int row, const int col) { return IsPointValid(row, col) && !IsPointClosed(row, col); }
inline bool IsPointCanPass(const int row, const int col) { return IsPointValid(row, col) && mapObstacles.find(GetPointID(row, col)) == mapObstacles.end(); }
inline int GetPointHScore(const int row, const int col) { return abs(row - tarRow) + abs(col - tarCol); }


struct PointRecord_DFS
{
	int row;
	int col;
	int hScore;
	PointRecord_DFS() {
		row = 0;
		col = 0;
		hScore = 0;
	}
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
	bool operator >=(const PointRecord_DFS &other) const
	{
		return hScore >= other.hScore;
	}
};

struct PointRecord_BFS
{
	int row;
	int col;
	int hScore;
	int parentIndex;
	PointRecord_BFS() {
		row = 0;
		col = 0;
		hScore = 0;
		parentIndex = 0;
	}
	PointRecord_BFS(int r, int c, int parent)
	{
		row = r;
		col = c;
		parentIndex = parent;
		hScore = GetPointHScore(r, c);
	}
	bool operator <(const PointRecord_BFS &other) const
	{
		return hScore < other.hScore;
	}
	bool operator >=(const PointRecord_BFS &other) const
	{
		return hScore >= other.hScore;
	}
};

vector<PointRecord_BFS> vecOpenNodes_BFS;

EX(int) TestMethod()
{
	return 10086;
}

EX(void) InitMap(int row, int col)
{
	mapObstacles.clear();
	maxRow = row;
	maxCol = col;
	listNavResult.resize((row + col) * 6);
	vecOpenNodes_BFS.reserve(row * col);
}

EX(void) SetPointObstacle(int row, int col)
{
	mapObstacles[GetPointID(row, col)] = true;
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

	//set<PointRecord_DFS> setPoints;
	//for (int i = 0; i < 4; ++i)
	//	if (IsPointValid(row + rowNeighbors[i], col + colNeighbors[i]))
	//		setPoints.insert(PointRecord_DFS(row + rowNeighbors[i], col + colNeighbors[i]));

	//for (set<PointRecord_DFS>::iterator it = setPoints.begin(); it != setPoints.end(); ++it)
	//{
	//	if (!IsPointClosed(it->row, it->col) && FindNextPoint(it->row, it->col))
	//	{
	//		listNavResult.push_back(it->row);
	//		listNavResult.push_back(it->col);
	//		return true;
	//	}
	//}

	MinHeap<PointRecord_DFS> heapNeighbors(4);
	for (int i = 0; i < 4; ++i)
		if (IsPointValid(row + rowNeighbors[i], col + colNeighbors[i]))
			heapNeighbors.Insert(PointRecord_DFS(row + rowNeighbors[i], col + colNeighbors[i]));
	for (int i = 0, length = heapNeighbors.size(); i < length; ++i)
	{
		PointRecord_DFS record = heapNeighbors.Extract();
		if (!IsPointClosed(record.row, record.col) && FindNextPoint(record.row, record.col))
		{
			listNavResult.push_back(row);
			listNavResult.push_back(col);
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
//DFS end


//BFS
EX(int*) Navigation_BFS(int startRow, int startCol, int targetRow, int targetCol, int* size)
{
	tarRow = targetRow;
	tarCol = targetCol;
	mapClosedNodes.clear();
	listNavResult.clear();
	vecOpenNodes_BFS.clear();
	vecOpenNodes_BFS.push_back(PointRecord_BFS(startRow, startCol, -1));
	vector<PointRecord_BFS> vecNeighbors(4);
	int curIndex = -1;
	while (++curIndex < vecOpenNodes_BFS.size())
	{
		const PointRecord_BFS& curRecord = vecOpenNodes_BFS[curIndex];
		if (IsPointClosed(curRecord.row, curRecord.col)) continue;
		ClosePoint(curRecord.row, curRecord.col);
		if (IsTargetPoint(curRecord.row, curRecord.col))
		{
			if (IsPointCanPass(curRecord.row, curRecord.col))
			{
				listNavResult.push_back(curRecord.row);
				listNavResult.push_back(curRecord.col);
			}
			curIndex = curRecord.parentIndex;
			do
			{
				listNavResult.push_back(vecOpenNodes_BFS[curIndex].row);
				listNavResult.push_back(vecOpenNodes_BFS[curIndex].col);
			} while ((curIndex = vecOpenNodes_BFS[curIndex].parentIndex) > -1);
			*size = listNavResult.size() / 2;
			return &listNavResult[0];
		}
		if (!IsPointCanPass(curRecord.row, curRecord.col)) continue;

		vecNeighbors.clear();
		for (int i = 0; i < 4; ++i)
			if (IsPointValidAndNotClosed(curRecord.row + rowNeighbors[i], curRecord.col + colNeighbors[i]))
				vecNeighbors.push_back(PointRecord_BFS(curRecord.row + rowNeighbors[i], curRecord.col + colNeighbors[i], curIndex));
		sort(vecNeighbors.begin(), vecNeighbors.end());

		for (int i = 0, length = vecNeighbors.size(); i < length; ++i)
			vecOpenNodes_BFS.push_back(vecNeighbors[i]);
	}
	*size = listNavResult.size() / 2;
	return &listNavResult[0];
}
//BFS end


//A_Star
//class NodeRecord
//{
//	public MapManager.NavPoint point;
//	public NodeRecord parentRecord;
//	public int hScore;
//	public int gScore;
//	public int fScore;
//	public NodeRecord(MapManager.NavPoint p, NodeRecord parent)
//	{
//		point = p;
//		fScore = (Mathf.Abs(p.row - curTarget.row) + Mathf.Abs(p.col - curTarget.col)) + (gScore = (parentRecord = parent) == null ? 0 : parent.gScore + 1);
//	}
//	public static implicit operator bool(NodeRecord record) { return record != null; }
//}
//
//static List<NodeRecord> listOpenNodes = new List<NodeRecord>(MapManager.MaxRow * MapManager.MaxCol);
//static Dictionary<int, NodeRecord> dicOpenNodes = new Dictionary<int, NodeRecord>();
//
//static MapManager.NavPoint curTarget;
//
EX(int*) Navigation_AStar(int startRow, int startCol, int targetRow, int targetCol, int* size)
{
	//tarRow = targetRow;
	//tarCol = targetCol;
	//mapClosedNodes.clear();
	//listNavResult.clear();
	//vecOpenNodes_BFS.clear();
	//	curTarget = target;
	//	dicClosedNodes.Clear();
	//	listNavResult.Clear();
	//	listOpenNodes.Clear();
	//	dicOpenNodes.Clear();
	//	NodeRecord record = new NodeRecord(start, null);
	//	listOpenNodes.Add(record);
	//	dicOpenNodes[start.id] = record;
	//	while (listOpenNodes.Count > 0)
	//	{
	//		NodeRecord curRecord = listOpenNodes[listOpenNodes.Count - 1];
	//		MapManager.NavPoint curPoint = curRecord.point;
	//		listOpenNodes.RemoveAt(listOpenNodes.Count - 1);
	//		dicOpenNodes.Remove(curPoint.id);
	//		if (dicClosedNodes.ContainsKey(curPoint.id)) continue;
	//		dicClosedNodes[curPoint.id] = true;

	//		if (curPoint == target)
	//		{
	//			do listNavResult.AddFirst(curRecord.point);
	//			while (curRecord = curRecord.parentRecord);
	//			return listNavResult;
	//		}
	//		if (curPoint.type != 0) continue;

	//		for (int i = 0, length = rowNeighbors.Length; i < length; ++i)
	//			AddChildPoint(MapManager.Instance.GetPoint(curPoint.row + rowNeighbors[i], curPoint.col + colNeighbors[i]), curRecord);
	//		listOpenNodes.Sort((a, b) = > { return b.fScore - a.fScore; });
	//	}
	*size = listNavResult.size() / 2;
	return &listNavResult[0];
}

//static void AddChildPoint(MapManager.NavPoint point, NodeRecord curRecord)
//{
//	if (!point || dicClosedNodes.ContainsKey(point.id)) return;
//	if (dicOpenNodes.ContainsKey(point.id))
//	{
//		NodeRecord record = dicOpenNodes[point.id];
//		if (curRecord.gScore < record.gScore)
//		{
//			record.parentRecord = curRecord;
//			record.fScore = record.hScore + (record.gScore = curRecord.gScore + 1);
//		}
//		return;
//	}
//	NodeRecord newRecord = new NodeRecord(point, curRecord);
//	listOpenNodes.Add(newRecord);
//	dicOpenNodes[point.id] = newRecord;
//}
