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
        public MeshFilter mfOperate;
        public Transform parent;
        public JudgeCellAvaliable cbIsGenerateCell;
    }

    const float CellWidth = 1;
    const float HalfCellWidth = 0.5f;
    const int GenerateLayer = 30;

    static List<Vector3> vertices = new List<Vector3>();
    static List<Vector2> UV = new List<Vector2>();
    static List<int> triangles = new List<int>();

    static Dictionary<int, bool> dicProcessedRecord = new Dictionary<int, bool>();

    static GenerateParams generateParams;
    static Vector3 vecOrigin = Vector3.zero;
    static int nRow = 100;
    static int nCol = 100;

    public string generateMaterialPath = "Assets/Resources/Materials/WallMat.mat";

    public static MeshFilter GenerateRectObj(Vector3 origin, GenerateParams gParams)
    {
        nRow = gParams.row;
        nCol = gParams.col;
        generateParams = gParams;
        vecOrigin = Vector3.zero;

        vertices.Clear();
        triangles.Clear();
        UV.Clear();
        dicProcessedRecord.Clear();

        int rectCount = 0;
        for (int i = 0; i < nCol; ++i)
            for (int j = 0; j < nRow; ++j)
                if (GenerateSingleRect(i, j))
                    ++rectCount;
        Debug.Log("Generate Rect Num: " + rectCount);

        Mesh mesh = null;
        if (gParams.mfOperate)
        {
            mesh = gParams.mfOperate.mesh;
            mesh.Clear();
        }
        else
        {
            GameObject go = new GameObject(gParams.objName);
            //分配mesh
            gParams.mfOperate = go.AddComponent<MeshFilter>();
            gParams.mfOperate.mesh = mesh = new Mesh();
            //分配材质
            MeshRenderer mr = go.GetComponent<MeshRenderer>();
            if (!mr) mr = go.AddComponent<MeshRenderer>();
            mr.sharedMaterial = AssetDatabase.LoadAssetAtPath<Material>(gParams.materialPath);

            if (gParams.parent) go.transform.SetParent(gParams.parent);
        }
        mesh.vertices = vertices.ToArray();
        mesh.triangles = triangles.ToArray();
        mesh.uv = UV.ToArray();

        mesh.RecalculateBounds();
        mesh.RecalculateTangents();
        mesh.RecalculateNormals();

        gParams.mfOperate.transform.position = origin;

        return gParams.mfOperate;
    }

    static bool GenerateSingleRect(int startX, int startZ)
    {
        if (dicProcessedRecord.ContainsKey(GetPointID(startZ, startX)) || !IsCellAvaliable(startZ, startX)) return false;
        int width = GetRowWidth(startX, startZ);
        int endZ = startZ + 1;
        for (; endZ < nRow; ++endZ)
            if (dicProcessedRecord.ContainsKey(GetPointID(endZ, startX)) || !IsCellAvaliable(endZ, startX) || GetRowWidth(startX, endZ) < width)
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
                dicProcessedRecord[GetPointID(j, startX + i)] = true;

        return true;
    }

    static int GetRowWidth(int startX, int z)
    {
        int colCounter = 0;
        for (int i = startX; i < nCol; ++i)
        {
            if (dicProcessedRecord.ContainsKey(GetPointID(z, i)) || !IsCellAvaliable(z, i))
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

    public static int GetPointID(int row, int col) { return row * nCol + col; }
}