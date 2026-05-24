using UnityEngine;

public class CraftingUI : MonoBehaviour
{
    [SerializeField] private CraftingManager _craftingManager;

    [Header("조합대 격자 규격 설정")]
    [SerializeField] private int _gridWidth = 3;   // 나중에 4x4로 늘리고 싶다면 인스펙터에서 4로 변경
    [SerializeField] private int _gridHeight = 3;  // 나중에 4x4로 늘리고 싶다면 인스펙터에서 4로 변경

    [Header("슬롯 UI 배열")]
    [SerializeField] private CraftingSlot[] _inputSlots; // 크기는 _gridWidth * _gridHeight와 같아야 함
    [SerializeField] private CraftingSlot _outputSlot;

    private void Start()
    {
        if (_inputSlots == null || _inputSlots.Length != _gridWidth * _gridHeight)
        {
            Debug.LogWarning("입력 슬롯 배열 크기가 설정된 Grid 크기(가로x세로)와 일치하지 않습니다!");
        }

        foreach (var slot in _inputSlots)
        {
            if (slot != null)
            {
                slot.OnSlotChanged += UpdateCrafting;
            }
        }
    }

    private void OnDestroy()
    {
        if (_inputSlots != null)
        {
            foreach (var slot in _inputSlots)
            {
                if (slot != null)
                {
                    slot.OnSlotChanged -= UpdateCrafting;
                }
            }
        }
    }

    private void UpdateCrafting()
    {
        int totalSlots = _gridWidth * _gridHeight;
        ItemData[] currentGrid = new ItemData[totalSlots];

        for (int i = 0; i < totalSlots; i++)
        {
            if (i < _inputSlots.Length)
            {
                currentGrid[i] = (_inputSlots[i] != null) ? _inputSlots[i].Item : null;
            }
        }

        if (_craftingManager != null && _craftingManager.HasMatchingRecipe(currentGrid, _gridWidth, _gridHeight, out ItemData result, out int count))
        {
            if (_outputSlot != null)
            {
                _outputSlot.SetItem(result, count);
            }
        }
        else
        {
            if (_outputSlot != null)
            {
                _outputSlot.ClearSlot();
            }
        }
    }

    public void Test_SetupGrid(ItemData[] testItems)
    {
        int totalSlots = _gridWidth * _gridHeight;
        for (int i = 0; i < Mathf.Min(testItems.Length, totalSlots); i++)
        {
            if (i < _inputSlots.Length && _inputSlots[i] != null)
            {
                _inputSlots[i].SetItem(testItems[i], 1);
            }
        }
        UpdateCrafting();
    }
}