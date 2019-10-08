// NavigationLab.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "NavigationLab.h"
using namespace std;

const int rowNeighbors[]{ -1, 1, 0, 0 };
const int colNeighbors[]{ 0, 0, -1, 1 };

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
	PointRecord_DFS(int r, int c, int parent, int hScore) :PointRecord(r, c, parent)
	{
		score = hScore;
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
	PointRecord_AStar(int r, int c, int parent, int h, int g) :PointRecord_DFS(r, c, parent, h + g)
	{
		gScore = g;
	}
};


//DFS
list<PointRecord_DFS> listOpenNodes_DFS;
map<int, PointRecord_DFS*> mapClosedNodes_DFS;
EX(int) Navigation_DFS(CINT startRow, CINT startCol, CINT targetRow, CINT targetCol, CINT maxRow, CINT maxCol, CINT* mapData, int* navPath)
{
	if (startRow == targetRow && startCol == targetCol) return 0;
	vector<PointRecord_DFS> vecNeighbors;
	vecNeighbors.reserve(4);
	listOpenNodes_DFS.push_back(PointRecord_DFS(startRow, startCol, -1, abs(startRow - targetRow) + abs(startCol - targetCol)));
	for (list<PointRecord_DFS>::iterator it = listOpenNodes_DFS.begin(), end = listOpenNodes_DFS.end(); it != end; ++it)
	{
		int curPointID = it->row * maxCol + it->col;
		if (mapClosedNodes_DFS.find(curPointID) != mapClosedNodes_DFS.end()) continue;
		mapClosedNodes_DFS[curPointID] = &*it;
		vecNeighbors.clear();
		for (int i = 0; i < 4; ++i)
		{
			int row = it->row + rowNeighbors[i], col = it->col + colNeighbors[i];
			if (row < 0 || row >= maxRow || col < 0 || col >= maxCol || mapClosedNodes_DFS.find(row * maxCol + col) != mapClosedNodes_DFS.end())
				continue;
			if (row == targetRow && col == targetCol)
			{
				int size = -1;
				if (mapData[row * maxCol + col])
				{
					navPath[++size] = row;
					navPath[++size] = col;
				}
				do
				{
					navPath[++size] = mapClosedNodes_DFS[curPointID]->row;
					navPath[++size] = mapClosedNodes_DFS[curPointID]->col;
				} while ((curPointID = mapClosedNodes_DFS[curPointID]->parentIndex) > -1);
				listOpenNodes_DFS.clear();
				mapClosedNodes_DFS.clear();
				return ++size / 2;
			}
			if (mapData[row * maxCol + col])
				vecNeighbors.push_back(PointRecord_DFS(row, col, curPointID, abs(row - targetRow) + abs(col - targetCol)));
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
	return 0;
}
//DFS end


//BFS
vector<PointRecord> vecOpenNodes_BFS;
map<int, bool> mapClosedPoints_BFS;
EX(int) Navigation_BFS(CINT startRow, CINT startCol, CINT targetRow, CINT targetCol, CINT maxRow, CINT maxCol, CINT* mapData, int* navPath)
{
	if (startRow == targetRow && startCol == targetCol) return 0;
	vecOpenNodes_BFS.reserve(maxRow * maxCol);
	vecOpenNodes_BFS.push_back(PointRecord(startRow, startCol, -1));
	for (int curIndex = 0; curIndex < vecOpenNodes_BFS.size(); ++curIndex)
	{
		if (mapClosedPoints_BFS.find(vecOpenNodes_BFS[curIndex].row * maxCol + vecOpenNodes_BFS[curIndex].col) != mapClosedPoints_BFS.end())
			continue;
		mapClosedPoints_BFS[vecOpenNodes_BFS[curIndex].row * maxCol + vecOpenNodes_BFS[curIndex].col] = true;
		for (int i = 0; i < 4; ++i)
		{
			int row = vecOpenNodes_BFS[curIndex].row + rowNeighbors[i], col = vecOpenNodes_BFS[curIndex].col + colNeighbors[i];
			if (row < 0 || row >= maxRow || col < 0 || col >= maxCol || mapClosedPoints_BFS.find(row * maxCol + col) != mapClosedPoints_BFS.end())
				continue;
			if (row == targetRow && col == targetCol)
			{
				int size = -1;
				if (mapData[row * maxCol + col])
				{
					navPath[++size] = row;
					navPath[++size] = col;
				}
				do
				{
					navPath[++size] = vecOpenNodes_BFS[curIndex].row;
					navPath[++size] = vecOpenNodes_BFS[curIndex].col;
				} while ((curIndex = vecOpenNodes_BFS[curIndex].parentIndex) > -1);
				mapClosedPoints_BFS.clear();
				vecOpenNodes_BFS.clear();
				return ++size / 2;
			}
			else if (mapData[row * maxCol + col])
				vecOpenNodes_BFS.push_back(PointRecord(row, col, curIndex));
		}
	}
	mapClosedPoints_BFS.clear();
	vecOpenNodes_BFS.clear();
	return 0;
}
//BFS end


//A_Star
MinHeap<PointRecord_AStar> heapOpenNodes_AStar;
map<int, PointRecord_AStar> mapClosedNodes_AStar;
EX(int) Navigation_AStar(CINT startRow, CINT startCol, CINT targetRow, CINT targetCol, CINT maxRow, CINT maxCol, CINT* mapData, int* navPath)
{
	if (startRow == targetRow && startCol == targetCol) return 0;
	heapOpenNodes_AStar.reserve(maxRow * maxCol);
	heapOpenNodes_AStar.Insert(PointRecord_AStar(startRow, startCol, -1, abs(startRow - targetRow) + abs(startCol - targetCol), 0));
	while (!heapOpenNodes_AStar.empty())
	{
		PointRecord_AStar curRecord = heapOpenNodes_AStar.Extract();
		int curPointID = curRecord.row * maxCol + curRecord.col;
		if (mapClosedNodes_AStar.find(curPointID) != mapClosedNodes_AStar.end()) continue;
		for (int i = 0; i < 4; ++i)
		{
			int row = curRecord.row + rowNeighbors[i], col = curRecord.col + colNeighbors[i];
			if (row < 0 || row >= maxRow || col < 0 || col >= maxCol || mapClosedNodes_AStar.find(row * maxCol + col) != mapClosedNodes_AStar.end())
				continue;
			if (row == targetRow && col == targetCol)
			{
				int size = -1;
				if (mapData[row * maxCol + col])
				{
					navPath[++size] = row;
					navPath[++size] = col;
				}
				navPath[++size] = curRecord.row;
				navPath[++size] = curRecord.col;
				for (int curIndex = curRecord.parentIndex; curIndex > -1; curIndex = mapClosedNodes_AStar[curIndex].parentIndex)
				{
					navPath[++size] = mapClosedNodes_AStar[curIndex].row;
					navPath[++size] = mapClosedNodes_AStar[curIndex].col;
				}
				heapOpenNodes_AStar.clear();
				mapClosedNodes_AStar.clear();
				return ++size / 2;
			}
			else if (mapData[row * maxCol + col])
				heapOpenNodes_AStar.Insert(PointRecord_AStar(row, col, curPointID, abs(row - targetRow) + abs(col - targetCol), curRecord.gScore + 1));
		}
		mapClosedNodes_AStar[curPointID] = std::move(curRecord);
	}
	heapOpenNodes_AStar.clear();
	mapClosedNodes_AStar.clear();
	return 0;
}
