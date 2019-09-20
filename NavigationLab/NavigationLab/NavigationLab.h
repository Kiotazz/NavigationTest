#pragma once
#include <map>
#include <vector>
#include <set>
#include <cmath>

#ifndef __NavLab__
#define __NavLab__
EXH(int) TestMethod();
EXH(void) InitMap(int row, int col);
EXH(void) SetPointObstacle(int row, int col);
EXH(int*) Navigation_DFS(int startRow, int startCol, int targetRow, int targetCol, int* size);
#endif