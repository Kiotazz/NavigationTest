// NavigationLab.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "NavigationLab.h"
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

inline bool IsTargetPoint(int row, int col) { return row == tarRow && col == tarCol; }
inline int GetPointID(int row, int col) { return row * maxCol + col; }
inline void ClosePoint(int row, int col) { mapClosedNodes[GetPointID(row, col)] = true; }
inline bool IsPointClosed(int row, int col) { return mapClosedNodes.find(GetPointID(row, col)) != mapClosedNodes.end(); }
inline bool IsPointValid(int row, int col) { return row > -1 && row < maxRow && col > -1 && col < maxCol; }
inline bool IsPointValidAndNotClosed(int row, int col) { return IsPointValid(row, col) && !IsPointClosed(row, col); }
inline bool IsPointCanPass(int row, int col) { return IsPointValid(row, col) && mapObstacles.find(GetPointID(row, col)) == mapObstacles.end(); }
inline int GetPointHScore(int row, int col) { return abs(row - tarRow) + abs(col - tarCol); }


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
};

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

	vector<PointRecord_DFS> vecPoints(4);
	for (int i = 0; i < 4; ++i)
		if (IsPointValid(row + rowNeighbors[i], col + colNeighbors[i]))
			vecPoints.push_back(PointRecord_DFS(row + rowNeighbors[i], col + colNeighbors[i]));
	sort(vecPoints.begin(), vecPoints.end());
	for (int i = 0, length = vecPoints.size(); i < length; ++i)
	{
		PointRecord_DFS& record = vecPoints[i];
		if (!IsPointClosed(record.row, record.col) && FindNextPoint(record.row, record.col))
		{
			listNavResult.push_back(record.row);
			listNavResult.push_back(record.col);
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
struct PointRecord_BFS
{
	int row;
	int col;
	int hScore;
	PointRecord_BFS* ptrParent;
	PointRecord_BFS() {
		row = 0;
		col = 0;
		hScore = 0;
		ptrParent = nullptr;
	}
	PointRecord_BFS(int r, int c, PointRecord_BFS* parent)
	{
		row = r;
		col = c;
		ptrParent = parent;
		hScore = GetPointHScore(r, c);
	}
	bool operator <(const PointRecord_BFS &other) const
	{
		return hScore < other.hScore;
	}
};

bool ComparerPointRecord_BFS(const PointRecord_BFS* left, const PointRecord_BFS* right)
{
	return left->hScore < right->hScore;
}

list<PointRecord_BFS*> listOpenNodes;
list<PointRecord_BFS*> listAllNodes;

void ClearNodesList()
{
	for (list<PointRecord_BFS*>::iterator it = listAllNodes.begin(); it != listAllNodes.end(); ++it)
		delete *it;
	listAllNodes.clear();
	listOpenNodes.clear();
}

EX(int*) Navigation_BFS(int startRow, int startCol, int targetRow, int targetCol, int* size)
{
	tarRow = targetRow;
	tarCol = targetCol;
	mapClosedNodes.clear();
	listNavResult.clear();
	PointRecord_BFS* record = new PointRecord_BFS(startRow, startCol, nullptr);
	listOpenNodes.push_back(record);
	listAllNodes.push_back(record);
	vector<PointRecord_BFS*> vecNeighbors(4);
	while (listOpenNodes.size() > 0)
	{
		PointRecord_BFS* curRecord = listOpenNodes.front();
		listOpenNodes.pop_front();
		if (IsPointClosed(curRecord->row, curRecord->col)) continue;
		ClosePoint(curRecord->row, curRecord->col);
		if (IsTargetPoint(curRecord->row, curRecord->col))
		{
			if (IsPointCanPass(curRecord->row, curRecord->col))
			{
				listNavResult.push_back(curRecord->row);
				listNavResult.push_back(curRecord->col);
			}
			while ((curRecord = curRecord->ptrParent) != nullptr)
			{
				listNavResult.push_back(curRecord->row);
				listNavResult.push_back(curRecord->col);
			}
			ClearNodesList();
			*size = listNavResult.size() / 2;
			return &listNavResult[0];
		}
		if (!IsPointCanPass(curRecord->row, curRecord->col)) continue;

		vecNeighbors.clear();
		for (int i = 0; i < 4; ++i)
		{
			if (IsPointValidAndNotClosed(curRecord->row + rowNeighbors[i], curRecord->col + colNeighbors[i]))
			{
				PointRecord_BFS* record = new PointRecord_BFS(curRecord->row + rowNeighbors[i], curRecord->col + colNeighbors[i], curRecord);
				listAllNodes.push_back(record);
				vecNeighbors.push_back(record);
			}
		}
		sort(vecNeighbors.begin(), vecNeighbors.end(), &ComparerPointRecord_BFS);

		for (int i = 0, length = vecNeighbors.size(); i < length; ++i)
			listOpenNodes.push_back(vecNeighbors[i]);
	}
	ClearNodesList();
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
	tarRow = targetRow;
	tarCol = targetCol;
	mapClosedNodes.clear();
	listNavResult.clear();
	listOpenNodes.clear();
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
