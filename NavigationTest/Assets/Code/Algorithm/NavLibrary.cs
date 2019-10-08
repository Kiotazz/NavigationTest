using System;
using System.Runtime.InteropServices;

public unsafe class NavLibrary
{
    public delegate int CB_CheckPointStatus(int row, int col);
    public enum Algorithm
    {
        DFS,
        BFS,
        A_Star
    }

    static int nMaxRow = 0;
    static int nMaxCol = 0;
    static Int32[] arrMapData;
    static Int32[] arrNavRoad;
    static Int32 nRoadSize = 0;

    public static void InitMap(int row, int col, CB_CheckPointStatus cbCheckPointStatus)
    {
        arrMapData = new Int32[(nMaxRow = row) * (nMaxCol = col)];
        arrNavRoad = new Int32[row * col * 2];
        for (int i = 0; i < row; ++i)
            for (int j = 0; j < col; ++j)
                arrMapData[i * col + j] = cbCheckPointStatus(i, j);
    }

    public static int GetPointID(int row, int col) { return row * nMaxCol + col; }

    public static void SetPointObstacle(int row, int col, bool isObstacle)
    {
        arrMapData[row * nMaxCol + col] = isObstacle ? 0 : 1;
    }

    public static bool IsPointObstacle(int row, int col)
    {
        return arrMapData[row * nMaxCol + col] == 0;
    }

    public static int Navigation(int startRow, int startCol, int targetRow, int targetCol, Algorithm algorithm = Algorithm.A_Star)
    {
        switch (algorithm)
        {
            case Algorithm.DFS:
                nRoadSize = Navigation_DFS(startRow, startCol, targetRow, targetCol, nMaxRow, nMaxCol, arrMapData, arrNavRoad);
                break;
            case Algorithm.BFS:
                nRoadSize = Navigation_BFS(startRow, startCol, targetRow, targetCol, nMaxRow, nMaxCol, arrMapData, arrNavRoad);
                break;
            case Algorithm.A_Star:
                nRoadSize = Navigation_AStar(startRow, startCol, targetRow, targetCol, nMaxRow, nMaxCol, arrMapData, arrNavRoad);
                break;
            default:
                nRoadSize = 0;
                break;
        }
        int roadSize = nRoadSize--;
        return roadSize;
    }

    [DllImport("NavigationLab")]
    static extern Int32 Navigation_DFS(int startRow, int startCol, int targetRow, int targetCol, int maxRow, int maxCol, Int32[] mapData, Int32[] road);

    [DllImport("NavigationLab")]
    static extern Int32 Navigation_BFS(int startRow, int startCol, int targetRow, int targetCol, int maxRow, int maxCol, Int32[] mapData, Int32[] road);

    [DllImport("NavigationLab")]
    static extern Int32 Navigation_AStar(int startRow, int startCol, int targetRow, int targetCol, int maxRow, int maxCol, Int32[] mapData, Int32[] road);

    public static bool GetPoint(Int32 index, ref Int32 row, ref Int32 col)
    {
        if ((index = nRoadSize - index) < 0) return false;
        row = arrNavRoad[index * 2];
        col = arrNavRoad[index * 2 + 1];
        return true;
    }
}
