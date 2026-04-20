using UnityEngine;
using UnityEngine.UI;
using UnityEngine.EventSystems;

namespace PoE_SkillTree
{
    /// <summary>
    /// 개별 노드 프리팹에 부착되어 상태 표시 및 클릭 처리 (UI Layer)
    /// </summary>
    public class SkillNodeUI : MonoBehaviour, IPointerClickHandler
    {
        public SkillNodeData data;

        [Header("UI Reference")]
        public Image iconImage;
        public Image frameImage;

        [Header("Visual State Colors")]
        public Color lockedColor = Color.gray;
        public Color availableColor = Color.white;
        public Color allocatedColor = Color.yellow;

        private void Start()
        {
            if (data != null)
            {
                if (iconImage != null) iconImage.sprite = data.icon;
                SkillTreeManager.Instance.OnTreeChanged += UpdateVisuals;
                UpdateVisuals();
            }
        }

        public void UpdateVisuals()
        {
            if (SkillTreeManager.Instance == null) return;

            bool isAllocated = SkillTreeManager.Instance.IsAllocated(data.id);
            bool canAllocate = SkillTreeManager.Instance.CanAllocate(data);

            if (isAllocated) frameImage.color = allocatedColor;
            else if (canAllocate) frameImage.color = availableColor;
            else frameImage.color = lockedColor;
        }

        public void OnPointerClick(PointerEventData eventData)
        {
            if (eventData.button == PointerEventData.InputButton.Left)
                SkillTreeManager.Instance.Allocate(data);
            else if (eventData.button == PointerEventData.InputButton.Right)
                SkillTreeManager.Instance.Deallocate(data);
        }
    }
}