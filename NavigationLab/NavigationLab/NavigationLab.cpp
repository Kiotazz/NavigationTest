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
vector<int> vecNavResult;

inline bool IsTargetPoint(const int row, const int col) { return row == tarRow && col == tarCol; }
inline int GetPointID(const int row, const int col) { return row * maxCol + col; }
inline bool IsPointValid(const int row, const int col) { return row > -1 && row < maxRow && col > -1 && col < maxCol; }
inline bool IsPointCanPass(const int row, const int col) { return mapObstacles.find(GetPointID(row, col)) == mapObstacles.end(); }
inline int GetPointHScore(const int row, const int col) { return abs(row - tarRow) + abs(col - tarCol); }


struct PointRecord
{
	int row;
	int col;
	int parentIndex;
	PointRecord() {
		row = 0;
		col = 0;
		parentIndex = -1;
	}
	PointRecord(int r, int c, int parent)
	{
		row = r;
		col = c;
		parentIndex = parent;
	}
};

struct PointRecord_DFS :public PointRecord
{
	int score;
	PointRecord_DFS() :PointRecord()
	{
		score = 0;
	}
	PointRecord_DFS(int r, int c, int parent) :PointRecord(r, c, parent)
	{
		score = GetPointHScore(r, c);
	}
	bool operator <(const PointRecord_DFS &other) const
	{
		return score < other.score;
	}
	bool operator >=(const PointRecord_DFS &other) const
	{
		return score >= other.score;
	}
};

struct PointRecord_AStar :public PointRecord_DFS
{
	int gScore;
	PointRecord_AStar() :PointRecord_DFS()
	{
		gScore = 0;
	}
	PointRecord_AStar(int r, int c, int parent, int g) :PointRecord_DFS(r, c, parent)
	{
		score += (gScore = g);
	}
};


list<PointRecord_DFS> listOpenNodes_DFS;
map<int, PointRecord_DFS*> mapClosedNodes_DFS;

vector<PointRecord> vecOpenNodes_BFS;
map<int, bool> mapClosedPoints_BFS;

MinHeap<PointRecord_AStar> heapOpenNodes_AStar;
map<int, PointRecord_AStar> mapClosedNodes_AStar;

EX(void) InitMap(int row, int col)
{
	mapObstacles.clear();
	maxRow = row;
	maxCol = col;
	vecNavResult.reserve((row + col) * 6);
	vecOpenNodes_BFS.reserve(row * col);
	heapOpenNodes_AStar.reserve(row * col);
}

EX(void) SetPointObstacle(int row, int col)
{
	mapObstacles[GetPointID(row, col)] = true;
}


//DFS
//bool FindNextPoint(int row, int col)
//{
//	ClosePoint(row, col);
//
//	vector<PointRecord> vecNeighbors;
//	vecNeighbors.reserve(4);
//	for (int i = 0; i < 4; ++i)
//		if (IsPointValid(row + rowNeighbors[i], col + colNeighbors[i]))
//			vecNeighbors.push_back(PointRecord(row + rowNeighbors[i], col + colNeighbors[i]));
//	sort(vecNeighbors.begin(), vecNeighbors.end());
//	for (int i = 0, length = vecNeighbors.size(); i < length; ++i)
//	{
//		const PointRecord& record = vecNeighbors[i];
//		if (IsTargetPoint(record.row, record.col))
//		{
//			if (IsPointCanPass(record.row, record.col))
//			{
//				vecNavResult.push_back(record.row);
//				vecNavResult.push_back(record.col);
//			}
//			vecNavResult.push_back(row);
//			vecNavResult.push_back(col);
//			return true;
//		}
//		if (IsPointCanPass(record.row, record.col) && !IsPointClosed(record.row, record.col) && FindNextPoint(record.row, record.col))
//		{
//			vecNavResult.push_back(row);
//			vecNavResult.push_back(col);
//			return true;
//		}
//	}
//	return false;
//}

EX(int*) Navigation_DFS(int startRow, int startCol, int targetRow, int targetCol, int& size)
{
	tarRow = targetRow;
	tarCol = targetCol;
	vecNavResult.clear();
	if (IsTargetPoint(startRow, startCol))
	{
		size = 0;
		return &vecNavResult[0];
	}
	//if (!IsTargetPoint(startRow, startCol))
	//	FindNextPoint(startRow, startCol);
	vector<PointRecord_DFS> vecNeighbors;
	vecNeighbors.reserve(4);
	listOpenNodes_DFS.push_back(PointRecord_DFS(startRow, startCol, -1));
	for (list<PointRecord_DFS>::iterator it = listOpenNodes_DFS.begin(), end = listOpenNodes_DFS.end(); it != end; ++it)
	{
		int curPointID = GetPointID(it->row, it->col);
		if (mapClosedNodes_DFS.find(curPointID) != mapClosedNodes_DFS.end()) continue;
		mapClosedNodes_DFS[curPointID] = &*it;
		vecNeighbors.clear();
		for (int i = 0; i < 4; ++i)
		{
			int row = it->row + rowNeighbors[i], col = it->col + colNeighbors[i];
			if (!IsPointValid(row, col) || mapClosedNodes_DFS.find(GetPointID(row, col)) != mapClosedNodes_DFS.end()) continue;
			if (IsTargetPoint(row, col))
			{
				if (IsPointCanPass(row, col))
				{
					vecNavResult.push_back(row);
					vecNavResult.push_back(col);
				}
				do
				{
					vecNavResult.push_back(mapClosedNodes_DFS[curPointID]->row);
					vecNavResult.push_back(mapClosedNodes_DFS[curPointID]->col);
				} while ((curPointID = mapClosedNodes_DFS[curPointID]->parentIndex) > -1);
				listOpenNodes_DFS.clear();
				mapClosedNodes_DFS.clear();
				size = vecNavResult.size() / 2;
				return &vecNavResult[0];
			}
			if (IsPointCanPass(row, col))
				vecNeighbors.push_back(PointRecord_DFS(row, col, curPointID));
		}
		sort(vecNeighbors.begin(), vecNeighbors.end());
		list<PointRecord_DFS>::iterator it2 = it;
		bool isEnd = ++it2 == end;
		for (int i = 0, length = vecNeighbors.size(); i < length; ++i)
		{
			if (isEnd)
				listOpenNodes_DFS.push_back(std::move(vecNeighbors[i]));
			else
				listOpenNodes_DFS.insert(it2, std::move(vecNeighbors[i]));
		}
	}
	listOpenNodes_DFS.clear();
	mapClosedNodes_DFS.clear();
	size = vecNavResult.size() / 2;
	return &vecNavResult[0];
}
//DFS end


//BFS
EX(int*) Navigation_BFS(int startRow, int startCol, int targetRow, int targetCol, int& size)
{
	tarRow = targetRow;
	tarCol = targetCol;
	vecNavResult.clear();
	if (IsTargetPoint(startRow, startCol))
	{
		size = 0;
		return &vecNavResult[0];
	}
	vecOpenNodes_BFS.push_back(PointRecord(startRow, startCol, -1));
	for (int curIndex = 0; curIndex < vecOpenNodes_BFS.size(); ++curIndex)
	{
		if (mapClosedPoints_BFS.find(GetPointID(vecOpenNodes_BFS[curIndex].row, vecOpenNodes_BFS[curIndex].col)) != mapClosedPoints_BFS.end())
			continue;
		mapClosedPoints_BFS[GetPointID(vecOpenNodes_BFS[curIndex].row, vecOpenNodes_BFS[curIndex].col)] = true;
		for (int i = 0; i < 4; ++i)
		{
			int row = vecOpenNodes_BFS[curIndex].row + rowNeighbors[i], col = vecOpenNodes_BFS[curIndex].col + colNeighbors[i];
			if (!IsPointValid(row, col) || mapClosedPoints_BFS.find(GetPointID(row, col)) != mapClosedPoints_BFS.end()) continue;
			if (IsTargetPoint(row, col))
			{
				if (IsPointCanPass(row, col))
				{
					vecNavResult.push_back(row);
					vecNavResult.push_back(col);
				}
				do
				{
					vecNavResult.push_back(vecOpenNodes_BFS[curIndex].row);
					vecNavResult.push_back(vecOpenNodes_BFS[curIndex].col);
				} while ((curIndex = vecOpenNodes_BFS[curIndex].parentIndex) > -1);
				mapClosedPoints_BFS.clear();
				vecOpenNodes_BFS.clear();
				size = vecNavResult.size() / 2;
				return &vecNavResult[0];
			}
			else if (IsPointCanPass(row, col))
				vecOpenNodes_BFS.push_back(PointRecord(row, col, curIndex));
		}
	}
	mapClosedPoints_BFS.clear();
	vecOpenNodes_BFS.clear();
	size = vecNavResult.size() / 2;
	return &vecNavResult[0];
}
//BFS end


//A_Star
EX(int*) Navigation_AStar(int startRow, int startCol, int targetRow, int targetCol, int& size)
{
	tarRow = targetRow;
	tarCol = targetCol;
	vecNavResult.clear();
	if (IsTargetPoint(startRow, startCol))
	{
		size = 0;
		return &vecNavResult[0];
	}
	heapOpenNodes_AStar.Insert(PointRecord_AStar(startRow, startCol, -1, 0));
	while (!heapOpenNodes_AStar.empty())
	{
		PointRecord_AStar curRecord = heapOpenNodes_AStar.Extract();
		if (mapClosedNodes_AStar.find(GetPointID(curRecord.row, curRecord.col)) != mapClosedNodes_AStar.end()) continue;
		int curPointID = GetPointID(curRecord.row, curRecord.col);
		for (int i = 0; i < 4; ++i)
		{
			int row = curRecord.row + rowNeighbors[i], col = curRecord.col + colNeighbors[i];
			if (!IsPointValid(row, col) || mapClosedNodes_AStar.find(GetPointID(row, col)) != mapClosedNodes_AStar.end()) continue;
			if (IsTargetPoint(row, col))
			{
				if (IsPointCanPass(row, col))
				{
					vecNavResult.push_back(row);
					vecNavResult.push_back(col);
				}
				vecNavResult.push_back(curRecord.row);
				vecNavResult.push_back(curRecord.col);
				for (int curIndex = curRecord.parentIndex; curIndex > -1; curIndex = mapClosedNodes_AStar[curIndex].parentIndex)
				{
					vecNavResult.push_back(mapClosedNodes_AStar[curIndex].row);
					vecNavResult.push_back(mapClosedNodes_AStar[curIndex].col);
				}
				heapOpenNodes_AStar.clear();
				mapClosedNodes_AStar.clear();
				size = vecNavResult.size() / 2;
				return &vecNavResult[0];
			}
			else if (IsPointCanPass(row, col))
				heapOpenNodes_AStar.Insert(PointRecord_AStar(row, col, curPointID, curRecord.gScore + 1));
		}
		mapClosedNodes_AStar[curPointID] = std::move(curRecord);
	}
	heapOpenNodes_AStar.clear();
	mapClosedNodes_AStar.clear();
	size = vecNavResult.size() / 2;
	return &vecNavResult[0];
}
