using UnityEngine;

[CreateAssetMenu(fileName = "NewItem", menuName = "Minecraft/Item")]
public class ItemData : ScriptableObject
{
    [SerializeField] private string _itemName;
    [SerializeField] private Sprite _icon;

    public string ItemName => _itemName;
    public Sprite Icon => _icon;
}