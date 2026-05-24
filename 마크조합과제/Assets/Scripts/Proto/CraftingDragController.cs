using UnityEngine;
using UnityEngine.UI;

/// <summary>
/// 드래그 중인 아이템의 비주얼 프리뷰(Icon) 및 데이터 흐름을 전역으로 관리하는 컨트롤러입니다.
/// </summary>
public class CraftingDragController : MonoBehaviour
{
    public static CraftingDragController Instance { get; private set; }

    [Header("드래그 프리뷰 UI")]
    [SerializeField] private Image _dragIcon;
    [SerializeField] private Canvas _canvas;

    private CraftingSlot _sourceSlot;
    private ItemData _draggedItem;
    private int _draggedCount;

    public bool IsDragging => _draggedItem != null;
    public CraftingSlot SourceSlot => _sourceSlot;
    public ItemData DraggedItem => _draggedItem;
    public int DraggedCount => _draggedCount;

    private void Awake()
    {
        if (Instance == null)
        {
            Instance = this;
        }
        else
        {
            Destroy(gameObject);
        }

        if (_dragIcon != null)
        {
            _dragIcon.gameObject.SetActive(false);
            _dragIcon.raycastTarget = false; // 드래그 중 Raycast 레이어 간섭 방지
        }
    }

    /// <summary>
    /// 드래그 연산을 개시합니다. PointerEventData의 실시간 좌표를 받아와 위치를 설정합니다.
    /// </summary>
    public void StartDrag(CraftingSlot source, ItemData item, int count, Vector2 screenPosition)
    {
        _sourceSlot = source;
        _draggedItem = item;
        _draggedCount = count;

        if (_dragIcon != null && item != null)
        {
            _dragIcon.sprite = item.Icon;
            _dragIcon.gameObject.SetActive(true);
            UpdatePosition(screenPosition);
        }
    }

    /// <summary>
    /// 드래그 이동을 갱신합니다.
    /// </summary>
    public void Dragging(Vector2 screenPosition)
    {
        UpdatePosition(screenPosition);
    }

    /// <summary>
    /// 드래그 연산을 정상 또는 비정상 종료합니다.
    /// </summary>
    public void EndDrag()
    {
        _sourceSlot = null;
        _draggedItem = null;
        _draggedCount = 0;

        if (_dragIcon != null)
        {
            _dragIcon.gameObject.SetActive(false);
        }
    }

    /// <summary>
    /// 스크린 포인터 좌표를 Canvas 내부 로컬 좌표계로 완벽히 투영합니다.
    /// </summary>
    private void UpdatePosition(Vector2 screenPosition)
    {
        if (_dragIcon == null || _canvas == null) return;

        Vector2 localPos;
        RectTransformUtility.ScreenPointToLocalPointInRectangle(
            _canvas.transform as RectTransform,
            screenPosition,
            _canvas.renderMode == RenderMode.ScreenSpaceOverlay ? null : Camera.main,
            out localPos
        );
        _dragIcon.rectTransform.anchoredPosition = localPos;
    }
}