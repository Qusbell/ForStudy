using System.Collections.Generic;
using UnityEngine;

public class CraftingManager : MonoBehaviour
{
    [SerializeField] private List<RecipeData> _recipes;

    /// <summary>
    /// 현재 조합대 격자(Grid)의 아이템 구성이 등록된 레시피와 일치하는지 검사합니다.
    /// 이 메서드는 조합대 크기(3x3, 4x4 등)에 구애받지 않고 작동합니다.
    /// </summary>
    public bool HasMatchingRecipe(ItemData[] grid, int gridWidth, int gridHeight, out ItemData result, out int count)
    {
        result = null;
        count = 0;

        // 1. 플레이어가 올려둔 아이템들의 바운딩 박스(최소 사각형 영역) 계산
        if (!GetGridBounds(grid, gridWidth, gridHeight, out int minX, out int maxX, out int minY, out int maxY))
        {
            return false; // 조합대에 아이템이 하나도 없음
        }

        int inputWidth = maxX - minX + 1;
        int inputHeight = maxY - minY + 1;

        // 2. 모든 레시피를 순회하며 규격 및 패턴 매칭 검사
        foreach (var recipe in _recipes)
        {
            if (recipe == null) continue;

            // 크기가 일치하는지 먼저 검사
            if (recipe.RecipeWidth == inputWidth && recipe.RecipeHeight == inputHeight)
            {
                if (CheckPatternMatch(grid, gridWidth, recipe, minX, minY, inputWidth, inputHeight))
                {
                    result = recipe.ResultItem;
                    count = recipe.ResultCount;
                    return true;
                }
            }
        }

        return false;
    }

    /// <summary>
    /// 실제 아이템이 배치된 영역의 상하좌우 경계(인덱스)를 찾습니다.
    /// </summary>
    private bool GetGridBounds(ItemData[] grid, int width, int height, out int minX, out int maxX, out int minY, out int maxY)
    {
        minX = width;
        maxX = -1;
        minY = height;
        maxY = -1;

        bool hasItem = false;

        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                int index = y * width + x;
                if (grid[index] != null)
                {
                    if (x < minX) minX = x;
                    if (x > maxX) maxX = x;
                    if (y < minY) minY = y;
                    if (y > maxY) maxY = y;
                    hasItem = true;
                }
            }
        }

        return hasItem;
    }

    /// <summary>
    /// 잘라낸 입력 영역의 아이템 배치 패턴이 레시피와 1:1로 일치하는지 검증합니다.
    /// </summary>
    private bool CheckPatternMatch(ItemData[] grid, int gridWidth, RecipeData recipe, int startX, int startY, int width, int height)
    {
        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                // 실제 전체 조합대에서의 위치 계산
                int gridIndex = (startY + y) * gridWidth + (startX + x);
                // 레시피 내부(Sub-Grid)에서의 위치 계산
                int recipeIndex = y * width + x;

                if (grid[gridIndex] != recipe.Ingredients[recipeIndex])
                {
                    return false; // 하나라도 다르면 매칭 실패
                }
            }
        }
        return true;
    }
}