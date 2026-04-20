using System;

namespace PoE_SkillTree
{
    /// <summary>
    /// 캐릭터의 능력치 종류 정의
    /// </summary>
    [Serializable]
    public enum StatType
    {
        Strength,
        Dexterity,
        Intelligence,
        Damage,
        Life
    }

    /// <summary>
    /// 개별 능력치 수정 데이터 (값과 종류)
    /// </summary>
    [Serializable]
    public class StatModifier
    {
        public StatType type;
        public float value;
    }
}