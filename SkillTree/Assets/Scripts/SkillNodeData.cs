using System.Collections.Generic;
using UnityEngine;

namespace PoE_SkillTree
{
    /// <summary>
    /// 각 스킬 노드의 고유 데이터를 정의하는 ScriptableObject
    /// </summary>
    [CreateAssetMenu(fileName = "NewSkillNode", menuName = "SkillTree/SkillNodeData")]
    public class SkillNodeData : ScriptableObject
    {
        [Header("기본 정보")]
        public string id;           // 고유 식별자 (저장 시 사용)
        public string skillName;    // 스킬 이름
        [TextArea] public string description; // 스킬 설명
        public Sprite icon;         // UI 아이콘

        [Header("능력치 및 연결")]
        public List<StatModifier> modifiers;  // 보너스 능력치 리스트
        public List<SkillNodeData> neighbors; // 인접 노드 리스트 (연결 관계)
        public bool isStartNode;              // 트리의 시작점 여부
    }
}