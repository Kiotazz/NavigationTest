#pragma once
#include <map>
#include <list>
#include <cmath>
#include "MinHeap.h"

#ifndef __NavLab__
#define __NavLab__
EXH(int) Navigation_DFS(CINT startRow, CINT startCol, CINT targetRow, CINT targetCol, CINT maxRow, CINT maxCol, CINT* mapData, int* navPath);
EXH(int) Navigation_BFS(CINT startRow, CINT startCol, CINT targetRow, CINT targetCol, CINT maxRow, CINT maxCol, CINT* mapData, int* navPath);
EXH(int) Navigation_AStar(CINT startRow, CINT startCol, CINT targetRow, CINT targetCol, CINT maxRow, CINT maxCol, CINT* mapData, int* navPath);
#endif