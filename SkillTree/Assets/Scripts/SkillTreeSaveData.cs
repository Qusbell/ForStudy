using System;
using System.Collections.Generic;

namespace PoE_SkillTree
{
    /// <summary>
    /// PlayerPrefs 또는 JSON 저장을 위한 데이터 구조 (포인트 필드 추가)
    /// </summary>
    [Serializable]
    public class SkillTreeSaveData
    {
        public int availablePoints; // 남은 스킬 포인트 저장
        public List<string> allocatedNodeIds = new List<string>();
    }
}