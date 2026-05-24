using System;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.EventSystems;

/// <summary>
/// 개별 조합 슬롯의 데이터 표현 및 EventSystem 기반의 드래그 앤 드롭 조작을 담당합니다.
/// </summary>
public class CraftingSlot : MonoBehaviour, IBeginDragHandler, IDragHandler, IEndDragHandler, IDropHandler
{
    public enum SlotType
    {
        Input,  // 재료 투입 격자 및 유저 인벤토리
        Output  // 조합 결과물 확인 및 수령
    }

    [Header("슬롯 속성 설정")]
    [SerializeField] private SlotType _slotType = SlotType.Input;

    [Header("UI 구성요소")]
    [SerializeField] private Image _itemIcon;
    [SerializeField] private Text _nameText;
    [SerializeField] private Text _countText;

    private ItemData _item;
    private int _count;

    public event Action OnSlotChanged;
    public event Action OnOutputTaken; // 결과물을 슬롯에서 성공적으로 꺼냈을 때 발생시킬 이벤트

    public ItemData Item => _item;
    public string Name => _item != null ? (!string.IsNullOrEmpty(_item.ItemName) ? _item.ItemName : _item.name) : string.Empty;
    public int Count => _count;
    public bool IsEmpty => _item == null;
    public SlotType Type => _slotType;

    public void SetItem(ItemData item, int count)
    {
        _item = item;
        _count = count;
        UpdateUI();
        OnSlotChanged?.Invoke();
    }

    public void ClearSlot()
    {
        _item = null;
        _count = 0;
        UpdateUI();
        OnSlotChanged?.Invoke();
    }

    private void UpdateUI()
    {
        if (IsEmpty)
        {
            if (_itemIcon != null) _itemIcon.gameObject.SetActive(false);
            if (_nameText != null) _nameText.gameObject.SetActive(false);
            if (_countText != null) _countText.gameObject.SetActive(false);
        }
        else
        {
            if (_itemIcon != null)
            {
                _itemIcon.gameObject.SetActive(true);
                _itemIcon.sprite = _item.Icon;
            }

            if (_nameText != null)
            {
                _nameText.gameObject.SetActive(true);
                _nameText.text = Name;
            }

            if (_countText != null)
            {
                if (_count > 1)
                {
                    _countText.gameObject.SetActive(true);
                    _countText.text = _count.ToString();
                }
                else
                {
                    _countText.gameObject.SetActive(false);
                }
            }
        }
    }

    // --- EventSystem 드래그 인터페이스 구현부 ---

    public void OnBeginDrag(PointerEventData eventData)
    {
        if (IsEmpty || CraftingDragController.Instance == null) return;

        ItemData draggedItem = _item;
        int draggedCount = _count;

        if (_slotType == SlotType.Input)
        {
            // 입력 슬롯은 꺼내갈 때 바로 비우고 드래그를 진행합니다.
            ClearSlot();
            CraftingDragController.Instance.StartDrag(this, draggedItem, draggedCount, eventData.position);
        }
        else if (_slotType == SlotType.Output)
        {
            // 결과 창은 프리뷰 특성상 미리 비우지 않고, 복제된 정보만 가지고 드래그를 시작합니다.
            CraftingDragController.Instance.StartDrag(this, draggedItem, draggedCount, eventData.position);
        }
    }

    public void OnDrag(PointerEventData eventData)
    {
        if (CraftingDragController.Instance != null && CraftingDragController.Instance.IsDragging)
        {
            CraftingDragController.Instance.Dragging(eventData.position);
        }
    }

    public void OnEndDrag(PointerEventData eventData)
    {
        if (CraftingDragController.Instance == null) return;

        // 허공에 마우스를 떼어 정상 드롭이 불가능해졌을 경우 원래 슬롯으로 롤백합니다.
        if (CraftingDragController.Instance.IsDragging)
        {
            CraftingSlot source = CraftingDragController.Instance.SourceSlot;
            if (source == this && _slotType == SlotType.Input)
            {
                SetItem(CraftingDragController.Instance.DraggedItem, CraftingDragController.Instance.DraggedCount);
            }
            CraftingDragController.Instance.EndDrag();
        }
    }

    public void OnDrop(PointerEventData eventData)
    {
        if (CraftingDragController.Instance == null || !CraftingDragController.Instance.IsDragging) return;

        // 결과 슬롯은 아이템을 놓을 수 있는 최종 목적지(Drop target)가 될 수 없습니다.
        if (_slotType == SlotType.Output) return;

        CraftingSlot source = CraftingDragController.Instance.SourceSlot;
        ItemData draggedItem = CraftingDragController.Instance.DraggedItem;
        int draggedCount = CraftingDragController.Instance.DraggedCount;

        if (IsEmpty)
        {
            // 1. 목적지 슬롯이 완전히 비어있을 때
            SetItem(draggedItem, draggedCount);

            if (source != null && source.Type == SlotType.Output)
            {
                // 소스가 결과창인 경우, 드롭이 무사히 안착되었을 때만 실제 재료 소모 트리거를 전달합니다.
                source.NotifyOutputTaken();
            }

            CraftingDragController.Instance.EndDrag();
        }
        else
        {
            // 2. 목적지 슬롯이 차있을 때
            if (source != null && source.Type == SlotType.Input)
            {
                // 마인크래프트 방식대로 입력 슬롯 간 스왑(Swap) 수행
                ItemData tempItem = _item;
                int tempCount = _count;

                SetItem(draggedItem, draggedCount);
                source.SetItem(tempItem, tempCount);

                CraftingDragController.Instance.EndDrag();
            }
            // 소스가 결과창인 경우, 이미 배치된 격자 위에는 덮어쓸 수 없도록 자연스럽게 무시합니다.
        }
    }

    private void NotifyOutputTaken()
    {
        OnOutputTaken?.Invoke();
    }
}