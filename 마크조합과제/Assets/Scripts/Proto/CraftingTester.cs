using UnityEngine;

public class CraftingTester : MonoBehaviour
{
    [SerializeField] private CraftingUI _craftingUI;
    [SerializeField] private ItemData _coal;
    [SerializeField] private ItemData _stick;

    private void Start()
    {
        TestTorch();
    }

    [ContextMenu("Test Torch Recipe")]
    public void TestTorch()
    {
        // 3x3 격자에서 한가운데 열에 [석탄, 빈칸, 빈칸 / 막대기, 빈칸, 빈칸 ...] 형태로 배치하여 테스트
        ItemData[] testGrid = new ItemData[9];
        testGrid[1] = _coal;  // 첫 번째 행 중간에 석탄
        testGrid[4] = _stick; // 두 번째 행 중간에 막대기

        _craftingUI.Test_SetupGrid(testGrid);
    }
}