using UnityEngine;
using UnityEngine.UI;

namespace PoE_SkillTree
{
    /// <summary>
    /// 두 노드 사이의 시각적 연결선 관리
    /// </summary>
    public class SkillTreeConnector : MonoBehaviour
    {
        private SkillNodeUI _nodeA;
        private SkillNodeUI _nodeB;
        private RectTransform _lineRect;
        private Image _lineImage;

        public Color inactiveColor = new Color(0.2f, 0.2f, 0.2f, 0.8f);
        public Color activeColor = Color.yellow;

        public void Setup(SkillNodeUI a, SkillNodeUI b, GameObject linePrefab)
        {
            _nodeA = a;
            _nodeB = b;

            GameObject lineObj = Instantiate(linePrefab, transform);
            _lineRect = lineObj.GetComponent<RectTransform>();
            _lineImage = lineObj.GetComponent<Image>();
            lineObj.transform.SetAsFirstSibling(); // 선을 노드 뒤로 배치

            SkillTreeManager.Instance.OnTreeChanged += UpdateLine;
            UpdateLine();
        }

        private void UpdateLine()
        {
            if (_nodeA == null || _nodeB == null) return;

            Vector2 posA = _nodeA.GetComponent<RectTransform>().anchoredPosition;
            Vector2 posB = _nodeB.GetComponent<RectTransform>().anchoredPosition;
            Vector2 dir = posB - posA;

            _lineRect.anchoredPosition = posA + dir * 0.5f;
            _lineRect.sizeDelta = new Vector2(dir.magnitude, 6f);
            _lineRect.localRotation = Quaternion.Euler(0, 0, Mathf.Atan2(dir.y, dir.x) * Mathf.Rad2Deg);

            bool isActive = SkillTreeManager.Instance.IsAllocated(_nodeA.data.id) &&
                           SkillTreeManager.Instance.IsAllocated(_nodeB.data.id);
            _lineImage.color = isActive ? activeColor : inactiveColor;
        }
    }
}