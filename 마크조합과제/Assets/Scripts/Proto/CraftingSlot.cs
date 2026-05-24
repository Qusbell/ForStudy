using System;
using UnityEngine;
using UnityEngine.UI;

public class CraftingSlot : MonoBehaviour
{
    [SerializeField] private Image _itemIcon;
    [SerializeField] private Text _nameText;
    [SerializeField] private Text _countText;

    private ItemData _item;
    private int _count;

    public event Action OnSlotChanged;

    public ItemData Item => _item;

    public string Name => _item != null ? _item.name : string.Empty;
    public int Count => _count;
    public bool IsEmpty => _item == null;

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
                _nameText.text = _item.name;
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
}
