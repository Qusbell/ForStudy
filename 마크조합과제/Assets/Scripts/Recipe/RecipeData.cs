using UnityEngine;

[CreateAssetMenu(fileName = "NewRecipe", menuName = "Minecraft/Recipe")]
public class RecipeData : ScriptableObject
{
    [Header("조합법 규격 설정")]
    [SerializeField] private int _recipeWidth = 3;  // 조합법의 가로 크기 (예: 횃불은 1, 상자는 3)
    [SerializeField] private int _recipeHeight = 3; // 조합법의 세로 크기 (예: 횃불은 2, 상자는 3)

    [Header("조합법 아이템 배열 (가로 x 세로 크기여야 함)")]
    [SerializeField] private ItemData[] _ingredients;

    [Header("결과물 설정")]
    [SerializeField] private ItemData _resultItem;
    [SerializeField] private int _resultCount = 1;

    public int RecipeWidth => _recipeWidth;
    public int RecipeHeight => _recipeHeight;
    public ItemData[] Ingredients => _ingredients;
    public ItemData ResultItem => _resultItem;
    public int ResultCount => _resultCount;

    // 에디터에서 값 검증용 (가로 x 세로 크기와 배열 크기가 맞는지 확인)
    private void OnValidate()
    {
        int requiredSize = _recipeWidth * _recipeHeight;
        if (_ingredients == null || _ingredients.Length != requiredSize)
        {
            System.Array.Resize(ref _ingredients, requiredSize);
        }
    }
}