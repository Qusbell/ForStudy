using System.Collections.Generic;
using UnityEngine;
using UnityEngine.EventSystems;
using UnityEngine.UI;

/// <summary>
/// 드롭다운에서 선택한 아이템을 무한 생성하고, 필요 없는 아이템을 끌어다 놓아 삭제할 수 있는 개발 테스트 전용 인벤토리 스크립트입니다.
/// </summary>
public class InfiniteInventory : MonoBehaviour, IDropHandler
{
    [Header("UI 컴포넌트 연결")]
    [SerializeField] private Dropdown _itemDropdown;         // 아이템 선택용 UI 드롭다운
    [SerializeField] private CraftingSlot _spawnerSlot;      // 아이템이 끊임없이 충전되는 슬롯

    [Header("전체 아이템 데이터 리스트")]
    [SerializeField] private List<ItemData> _availableItems; // 게임에 등록된 전체 ItemData 에셋 목록

    private ItemData _currentItem;
    private bool _isUpdating; // 재귀 호출 방지 플래그

    private void Start()
    {
        InitializeDropdown();

        if (_spawnerSlot != null)
        {
            _spawnerSlot.OnSlotChanged += HandleSpawnerSlotChanged;
        }

        UpdateSpawnerSlot();
    }

    private void OnDestroy()
    {
        if (_spawnerSlot != null)
        {
            _spawnerSlot.OnSlotChanged -= HandleSpawnerSlotChanged;
        }
    }

    /// <summary>
    /// 등록된 ItemData들을 기반으로 드롭다운 UI 옵션을 자동 세팅합니다.
    /// </summary>
    private void InitializeDropdown()
    {
        if (_itemDropdown == null) return;

        _itemDropdown.ClearOptions();
        List<string> options = new List<string>();

        foreach (var item in _availableItems)
        {
            if (item != null)
            {
                // ItemData의 _itemName 필드가 비어있다면 ScriptableObject 에셋 파일 이름을 기본값으로 설정
                string displayName = !string.IsNullOrEmpty(item.ItemName) ? item.ItemName : item.name;
                options.Add(displayName);
            }
        }

        _itemDropdown.AddOptions(options);
        _itemDropdown.onValueChanged.AddListener(OnDropdownValueChanged);
    }

    private void OnDropdownValueChanged(int index)
    {
        UpdateSpawnerSlot();
    }

    /// <summary>
    /// 드롭다운에서 선택된 아이템을 생성기 슬롯에 채웁니다.
    /// </summary>
    private void UpdateSpawnerSlot()
    {
        if (_itemDropdown == null || _availableItems == null || _availableItems.Count == 0) return;

        int selectedIndex = _itemDropdown.value;
        if (selectedIndex >= 0 && selectedIndex < _availableItems.Count)
        {
            _currentItem = _availableItems[selectedIndex];
            if (_spawnerSlot != null)
            {
                _isUpdating = true;
                _spawnerSlot.SetItem(_currentItem, 1);
                _isUpdating = false;
            }
        }
    }

    /// <summary>
    /// 사용자가 생성기 슬롯에서 아이템을 드래그해 꺼내갔을 때 작동하는 이벤트 리스너입니다.
    /// </summary>
    private void HandleSpawnerSlotChanged()
    {
        if (_isUpdating) return;

        // 슬롯이 비는 즉시 동일한 아이템을 무한히 다시 채워 넣습니다.
        if (_spawnerSlot != null && _spawnerSlot.IsEmpty && _currentItem != null)
        {
            _isUpdating = true;
            _spawnerSlot.SetItem(_currentItem, 1);
            _isUpdating = false;
        }
    }

    /// <summary>
    /// 무한 인벤토리 패널 배경 영역에 마우스를 드롭하면 호출됩니다.
    /// </summary>
    public void OnDrop(PointerEventData eventData)
    {
        if (CraftingDragController.Instance == null || !CraftingDragController.Instance.IsDragging) return;

        // 드래그가 시작될 때 원본 슬롯은 이미 비워져 있으므로,
        // 어디에도 드롭시키지 않고 여기서 드래그를 강제 종료하면 아이템이 자연스럽게 "삭제" 처리됩니다.
        CraftingDragController.Instance.EndDrag();
    }
}