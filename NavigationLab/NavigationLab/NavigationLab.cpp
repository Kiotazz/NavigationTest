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
map<int, bool> mapClosedPoints;

inline bool IsTargetPoint(const int row, const int col) { return row == tarRow && col == tarCol; }
inline int GetPointID(const int row, const int col) { return row * maxCol + col; }
inline void ClosePoint(const int row, const int col) { mapClosedPoints[GetPointID(row, col)] = true; }
inline bool IsPointClosed(const int row, const int col) { return mapClosedPoints.find(GetPointID(row, col)) != mapClosedPoints.end(); }
inline bool IsPointValid(const int row, const int col) { return row > -1 && row < maxRow && col > -1 && col < maxCol; }
inline bool IsPointInvalidOrClosed(const int row, const int col) { return !IsPointValid(row, col) || IsPointClosed(row, col); }
inline bool IsPointCanPass(const int row, const int col) { return mapObstacles.find(GetPointID(row, col)) == mapObstacles.end(); }
inline int GetPointHScore(const int row, const int col) { return abs(row - tarRow) + abs(col - tarCol); }


struct PointRecord
{
	int row;
	int col;
	int score;
	PointRecord() {
		row = 0;
		col = 0;
		score = 0;
	}
	PointRecord(int r, int c)
	{
		row = r;
		col = c;
		score = GetPointHScore(r, c);
	}
	bool operator <(const PointRecord &other) const
	{
		return score < other.score;
	}
	bool operator >=(const PointRecord &other) const
	{
		return score >= other.score;
	}
};

struct PointRecord_BFS :public PointRecord
{
	int parentIndex;
	PointRecord_BFS() :PointRecord()
	{
		parentIndex = -1;
	}
	PointRecord_BFS(int r, int c, int parent) :PointRecord(r, c)
	{
		parentIndex = parent;
	}
};

struct PointRecord_AStar :public PointRecord_BFS
{
	int gScore;
	PointRecord_AStar() :PointRecord_BFS()
	{
		gScore = 0;
	}
	PointRecord_AStar(int r, int c, int parent, int g) :PointRecord_BFS(r, c, parent)
	{
		score += (gScore = g);
	}
};

vector<PointRecord_BFS> vecOpenNodes_BFS;
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
bool FindNextPoint(int row, int col)
{
	ClosePoint(row, col);

	vector<PointRecord> vecNeighbors;
	vecNeighbors.reserve(4);
	for (int i = 0; i < 4; ++i)
		if (IsPointValid(row + rowNeighbors[i], col + colNeighbors[i]))
			vecNeighbors.push_back(PointRecord(row + rowNeighbors[i], col + colNeighbors[i]));
	sort(vecNeighbors.begin(), vecNeighbors.end());
	for (int i = 0, length = vecNeighbors.size(); i < length; ++i)
	{
		const PointRecord& record = vecNeighbors[i];
		if (IsTargetPoint(record.row, record.col))
		{
			if (IsPointCanPass(record.row, record.col))
			{
				vecNavResult.push_back(record.row);
				vecNavResult.push_back(record.col);
			}
			vecNavResult.push_back(row);
			vecNavResult.push_back(col);
			return true;
		}
		if (IsPointCanPass(record.row, record.col) && !IsPointClosed(record.row, record.col) && FindNextPoint(record.row, record.col))
		{
			vecNavResult.push_back(row);
			vecNavResult.push_back(col);
			return true;
		}
	}
	return false;
}

EX(int*) Navigation_DFS(int startRow, int startCol, int targetRow, int targetCol, int& size)
{
	tarRow = targetRow;
	tarCol = targetCol;
	vecNavResult.clear();
	if (!IsTargetPoint(startRow, startCol))
		FindNextPoint(startRow, startCol);
	mapClosedPoints.clear();
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
	vecOpenNodes_BFS.push_back(PointRecord_BFS(startRow, startCol, -1));
	vector<PointRecord_BFS> vecNeighbors;
	vecNeighbors.reserve(4);
	int curIndex = -1;
	while (++curIndex < vecOpenNodes_BFS.size())
	{
		const PointRecord_BFS& curRecord = vecOpenNodes_BFS[curIndex];
		if (IsPointClosed(curRecord.row, curRecord.col)) continue;
		ClosePoint(curRecord.row, curRecord.col);
		vecNeighbors.clear();
		for (int i = 0; i < 4; ++i)
		{
			int row = curRecord.row + rowNeighbors[i], col = curRecord.col + colNeighbors[i];
			if (IsPointInvalidOrClosed(row, col)) continue;
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
				mapClosedPoints.clear();
				vecOpenNodes_BFS.clear();
				size = vecNavResult.size() / 2;
				return &vecNavResult[0];
			}
			else if (IsPointValid(row, col) && IsPointCanPass(row, col))
				vecNeighbors.push_back(PointRecord_BFS(row, col, curIndex));
		}
		sort(vecNeighbors.begin(), vecNeighbors.end());
		for (int i = 0, length = vecNeighbors.size(); i < length; ++i)
			vecOpenNodes_BFS.push_back(std::move(vecNeighbors[i]));
	}
	mapClosedPoints.clear();
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
		if (IsPointClosed(curRecord.row, curRecord.col)) continue;
		ClosePoint(curRecord.row, curRecord.col);
		int curPointID = GetPointID(curRecord.row, curRecord.col);
		for (int i = 0; i < 4; ++i)
		{
			int row = curRecord.row + rowNeighbors[i], col = curRecord.col + colNeighbors[i];
			if (IsPointInvalidOrClosed(row, col)) continue;
			if (IsTargetPoint(row, col))
			{
				if (IsPointCanPass(row, col))
				{
					vecNavResult.push_back(row);
					vecNavResult.push_back(col);
				}
				vecNavResult.push_back(curRecord.row);
				vecNavResult.push_back(curRecord.col);
				for (int curIndex = curRecord.parentIndex; curIndex > -1; )
				{
					const PointRecord_AStar& ptrRecord = mapClosedNodes_AStar[curIndex];
					vecNavResult.push_back(ptrRecord.row);
					vecNavResult.push_back(ptrRecord.col);
					curIndex = ptrRecord.parentIndex;
				}
				mapClosedPoints.clear();
				heapOpenNodes_AStar.clear();
				mapClosedNodes_AStar.clear();
				size = vecNavResult.size() / 2;
				return &vecNavResult[0];
			}
			else if (IsPointValid(row, col) && IsPointCanPass(row, col))
				heapOpenNodes_AStar.Insert(PointRecord_AStar(row, col, curPointID, curRecord.gScore + 1));
		}
		mapClosedNodes_AStar[curPointID] = std::move(curRecord);
	}
	mapClosedPoints.clear();
	heapOpenNodes_AStar.clear();
	mapClosedNodes_AStar.clear();
	size = vecNavResult.size() / 2;
	return &vecNavResult[0];
}
