using System;
using System.Collections.Generic;

namespace PoE_SkillTree
{
    /// <summary>
    /// PlayerPrefs 또는 JSON 저장을 위한 데이터 구조
    /// </summary>
    [Serializable]
    public class SkillTreeSaveData
    {
        public List<string> allocatedNodeIds = new List<string>();
    }
}