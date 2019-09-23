using UnityEngine.UI;
using UnityEngine;

public class MainView : MonoBehaviour
{
    public static MainView Instance;

    public InputField inputRow;
    public InputField inputCol;
    public Dropdown dropNavType;
    public Text txtTimeCost;

    private void Start()
    {
        Instance = this;
    }

    public void OnBtnConfirm()
    {
        int row = Mathf.Clamp(string.IsNullOrEmpty(inputRow.text) ? 0 : int.Parse(inputRow.text), 0, MapManager.MaxRow - 1);
        int col = Mathf.Clamp(string.IsNullOrEmpty(inputCol.text) ? 0 : int.Parse(inputCol.text), 0, MapManager.MaxCol - 1);
        inputRow.text = row.ToString();
        inputCol.text = col.ToString();
        MyNavAgent.Instance.SetTarget(row, col);
    }

    public void RefreshInfo(int row, int col, double time)
    {
        txtTimeCost.text = "用时：" + time + "ms";
        inputRow.text = row.ToString();
        inputCol.text = col.ToString();
        string navType = MyNavAgent.Instance.navType.ToString();
        for (int i = 0, length = dropNavType.options.Count; i < length; ++i)
        {
            if (dropNavType.options[i].text == navType)
            {
                dropNavType.value = i;
                break;
            }
        }
    }

    public void OnNavTypeChange()
    {
        switch (dropNavType.captionText.text)
        {
            case "DFS":
                MyNavAgent.Instance.navType = NavLibrary.Algorithm.DFS;
                break;
            case "BFS":
                MyNavAgent.Instance.navType = NavLibrary.Algorithm.BFS;
                break;
            default:
                MyNavAgent.Instance.navType = NavLibrary.Algorithm.A_Star;
                break;
        }
    }
}
