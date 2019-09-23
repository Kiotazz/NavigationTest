#pragma once
#include <map>
#include <list>
#include <cmath>
#include <algorithm>
#include "MinHeap.h"

#ifndef __NavLab__
#define __NavLab__
EXH(void) InitMap(int row, int col);
EXH(void) SetPointObstacle(int row, int col);
EXH(int*) Navigation_DFS(int startRow, int startCol, int targetRow, int targetCol, int& size);
EXH(int*) Navigation_BFS(int startRow, int startCol, int targetRow, int targetCol, int& size);
EXH(int*) Navigation_AStar(int startRow, int startCol, int targetRow, int targetCol, int& size);
#endif