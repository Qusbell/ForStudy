using UnityEngine;

/// <summary>
/// 조합 UI 판넬 내부의 전체 입력/출력 슬롯의 변경 상태를 감지하고 연산을 조율합니다.
/// </summary>
public class CraftingUI : MonoBehaviour
{
    [SerializeField] private CraftingManager _craftingManager;

    [Header("조합대 격자 규격 설정")]
    [SerializeField] private int _gridWidth = 3;
    [SerializeField] private int _gridHeight = 3;

    [Header("슬롯 UI 배열")]
    [SerializeField] private CraftingSlot[] _inputSlots;
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

        if (_outputSlot != null)
        {
            _outputSlot.OnOutputTaken += ConsumeIngredients;
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

        if (_outputSlot != null)
        {
            _outputSlot.OnOutputTaken -= ConsumeIngredients;
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

    /// <summary>
    /// 조합이 성공하여 결과 슬롯에서 아이템을 가져갔을 때, 조합대에 등록된 모든 재료를 1개씩 감소시킵니다.
    /// </summary>
    private void ConsumeIngredients()
    {
        if (_inputSlots == null) return;

        foreach (var slot in _inputSlots)
        {
            if (slot != null && !slot.IsEmpty)
            {
                int newCount = slot.Count - 1;
                if (newCount <= 0)
                {
                    slot.ClearSlot();
                }
                else
                {
                    slot.SetItem(slot.Item, newCount);
                }
            }
        }

        // 재료가 한 층 깎인 뒤 다시 새로운 프리뷰 결과를 업데이트합니다.
        UpdateCrafting();
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