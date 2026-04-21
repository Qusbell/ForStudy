using UnityEngine;
using UnityEngine.UI;

namespace PoE_SkillTree
{
    /// <summary>
    /// Viewport의 크기에 맞춰 외곽선을 표시합니다. 
    /// Viewport와 동일한 위치에 있는 별도 오브젝트에 부착하세요.
    /// </summary>
    [RequireComponent(typeof(RectTransform))]
    public class ViewportOutline : MonoBehaviour
    {
        [Header("설정")]
        public RectTransform targetViewport; // 추적할 Viewport
        public Color outlineColor = new Color(1f, 0.8f, 0.2f, 0.5f); // 외곽선 색상 (PoE풍 골드)
        public float outlineThickness = 2f; // 선 두께

        private Outline _outline;
        private Image _image;
        private RectTransform _rect;

        private void Awake()
        {
            _rect = GetComponent<RectTransform>();

            // 시각 효과를 위한 Image와 Outline 추가
            _image = gameObject.GetComponent<Image>();
            if (_image == null) _image = gameObject.AddComponent<Image>();

            // 이미지는 투명하게 (선만 보이게)
            _image.color = new Color(0, 0, 0, 0);
            _image.raycastTarget = false; // 클릭 방해 방지

            _outline = gameObject.GetComponent<Outline>();
            if (_outline == null) _outline = gameObject.AddComponent<Outline>();

            _outline.effectColor = outlineColor;
            _outline.effectDistance = new Vector2(outlineThickness, outlineThickness);
        }

        private void LateUpdate()
        {
            if (targetViewport == null) return;

            // Viewport의 크기와 앵커 정보를 실시간으로 복사
            _rect.anchorMin = targetViewport.anchorMin;
            _rect.anchorMax = targetViewport.anchorMax;
            _rect.pivot = targetViewport.pivot;
            _rect.sizeDelta = targetViewport.sizeDelta;
            _rect.anchoredPosition = targetViewport.anchoredPosition;
        }
    }
}