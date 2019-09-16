using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Text;
using System.IO;
#if UNITY_EDITOR
using UnityEditor;
#endif

public class RectMeshCreater : MonoBehaviour
{
    public const int MaxRow = 200;
    public const int MaxCol = 200;

    public delegate bool JudgeCellAvaliable(int row, int col);
    public struct GenerateParams
    {
        public int row;
        public int col;
        public int startRow;
        public int startCol;
        public string materialPath;
        public string objName;
        public Transform parent;
        public JudgeCellAvaliable cbIsGenerateCell;
    }

    const float CellWidth = 1;
    const float HalfCellWidth = 0.5f;
    const int GenerateLayer = 30;

    static List<Vector3> vertices;
    static List<Vector2> UV;
    static List<int> triangles;

    static Dictionary<int, Dictionary<int, bool>> dicProcessedRecord;

    static GenerateParams generateParams;
    static Vector3 vecOrigin = Vector3.zero;
    static int nRow = 100;
    static int nCol = 100;

    public string generateMaterialPath = "Assets/Resources/Materials/WallMat.mat";

    public static GameObject GenerateRectObj(Vector3 origin, GenerateParams gParams)
    {
        nRow = gParams.row;
        nCol = gParams.col;
        generateParams = gParams;
        vecOrigin = Vector3.zero;

        vertices = new List<Vector3>();
        triangles = new List<int>();
        UV = new List<Vector2>();
        dicProcessedRecord = new Dictionary<int, Dictionary<int, bool>>();
        for (int i = 0; i < nRow; ++i)
            dicProcessedRecord[i] = new Dictionary<int, bool>();

        int rectCount = 0;
        for (int i = 0; i < nCol; ++i)
            for (int j = 0; j < nRow; ++j)
                if (GenerateSingleRect(i, j))
                    ++rectCount;
        Debug.Log("Generate Rect Num: " + rectCount);

        Mesh mesh = new Mesh();
        mesh.vertices = vertices.ToArray();
        mesh.triangles = triangles.ToArray();
        mesh.uv = UV.ToArray();

        mesh.RecalculateBounds();
        mesh.RecalculateTangents();

        GameObject go = new GameObject();
        go.name = gParams.objName;
        //分配mesh
        MeshFilter mf = go.AddComponent<MeshFilter>();
        mf.mesh = mesh;
        mesh.RecalculateNormals();
        //分配材质
        MeshRenderer mr = go.GetComponent<MeshRenderer>();
        if (!mr) mr = go.AddComponent<MeshRenderer>();

        Material myMat = AssetDatabase.LoadAssetAtPath<Material>(gParams.materialPath);
        mr.sharedMaterial = myMat;

        go.transform.position = origin + new Vector3(0, 0.01f, 0);
        if (gParams.parent)
            go.transform.SetParent(gParams.parent);

        return go;
    }

    static bool GenerateSingleRect(int startX, int startZ)
    {
        if (dicProcessedRecord[startZ].ContainsKey(startX) || !IsCellAvaliable(startZ, startX)) return false;
        int width = GetRowWidth(startX, startZ);
        int endZ = startZ + 1;
        for (; endZ < nRow; ++endZ)
            if (dicProcessedRecord[endZ].ContainsKey(startX) || !IsCellAvaliable(endZ, startX) || GetRowWidth(startX, endZ) < width)
                break;

        Vector3 vecStartPos = vecOrigin;
        vecStartPos.x += CellWidth * startX;
        vecStartPos.z += CellWidth * startZ;

        Vector3 vecEndPos = vecOrigin;
        vecEndPos.x += CellWidth * (startX + width - 1);
        vecEndPos.z += CellWidth * (endZ - 1);

        int curIndex = vertices.Count;

        vertices.Add(new Vector3(vecStartPos.x - HalfCellWidth, 0, vecEndPos.z + HalfCellWidth));
        vertices.Add(new Vector3(vecEndPos.x + HalfCellWidth, 0, vecEndPos.z + HalfCellWidth));
        vertices.Add(new Vector3(vecEndPos.x + HalfCellWidth, 0, vecStartPos.z - HalfCellWidth));
        vertices.Add(new Vector3(vecStartPos.x - HalfCellWidth, 0, vecStartPos.z - HalfCellWidth));

        UV.Add(new Vector2(0, 0));
        UV.Add(new Vector2(width, 0));
        UV.Add(new Vector2(width, endZ - startZ));
        UV.Add(new Vector2(0, endZ - startZ));

        triangles.Add(curIndex); triangles.Add(curIndex + 1); triangles.Add(curIndex + 2);
        triangles.Add(curIndex); triangles.Add(curIndex + 2); triangles.Add(curIndex + 3);

        for (int i = 0; i < width; ++i)
            for (int j = startZ; j < endZ; ++j)
                dicProcessedRecord[j][startX + i] = true;

        return true;
    }

    static int GetRowWidth(int startX, int z)
    {
        int colCounter = 0;
        for (int i = startX; i < nCol; ++i)
        {
            if (dicProcessedRecord[z].ContainsKey(i) || !IsCellAvaliable(z, i))
                return colCounter;
            else
                ++colCounter;
        }
        return colCounter;
    }

    static bool IsCellAvaliable(int row, int col)
    {
        return generateParams.cbIsGenerateCell(generateParams.startRow + row, generateParams.startCol + col);
    }
}