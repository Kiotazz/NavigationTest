using System;
using System.Runtime.InteropServices;

public class NavLibrary
{
    public enum Algorithm
    {
        DFS,
        BFS,
        A_Star
    }

    static IntPtr nativeArrRoad;
    static int nRoadSize = 0;

    [DllImport("NavigationLab")]
    public static extern void InitMap(int row, int col);

    [DllImport("NavigationLab")]
    public static extern void SetPointObstacle(int row, int col);

    [DllImport("NavigationLab")]
    public static extern int TestMethod();

    [DllImport("NavigationLab")]
    static extern IntPtr Navigation_DFS(int startRow, int startCol, int targetRow, int targetCol, ref int size);

    [DllImport("NavigationLab")]
    static extern IntPtr Navigation_BFS(int startRow, int startCol, int targetRow, int targetCol, ref int size);

    [DllImport("NavigationLab")]
    static extern IntPtr Navigation_AStar(int startRow, int startCol, int targetRow, int targetCol, ref int size);

    public static int Navigation(int startRow, int startCol, int targetRow, int targetCol, Algorithm algorithm = Algorithm.A_Star)
    {
        switch (algorithm)
        {
            case Algorithm.DFS:
                nativeArrRoad = Navigation_DFS(startRow, startCol, targetRow, targetCol, ref nRoadSize);
                break;
            case Algorithm.BFS:
                nativeArrRoad = Navigation_BFS(startRow, startCol, targetRow, targetCol, ref nRoadSize);
                break;
            case Algorithm.A_Star:
                nativeArrRoad = Navigation_AStar(startRow, startCol, targetRow, targetCol, ref nRoadSize);
                break;
            default:
                nRoadSize = 0;
                break;
        }
        return nRoadSize--;
    }

    public static bool GetPoint(int index, ref int row, ref int col)
    {
        if ((index = nRoadSize - index) < 0) return false;
        row = Marshal.ReadInt32(nativeArrRoad, index * 2 * sizeof(int));
        col = Marshal.ReadInt32(nativeArrRoad, (index * 2 + 1) * sizeof(int));
        return true;
    }
}
