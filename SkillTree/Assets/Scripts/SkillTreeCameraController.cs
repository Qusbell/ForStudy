using UnityEngine;
using UnityEngine.InputSystem; // 새로운 입력 시스템 네임스페이스 추가

namespace PoE_SkillTree
{
    /// <summary>
    /// New Input System을 지원하며 우클릭 드래그 기능을 포함한 스킬트리 카메라 컨트롤러
    /// </summary>
    public class SkillTreeCameraController : MonoBehaviour
    {
        [Header("줌 설정")]
        public float zoomSpeed = 0.01f;
        public float minZoom = 0.3f;
        public float maxZoom = 1.6f;

        [Header("이동 설정")]
        public float panSpeed = 1.0f;

        private RectTransform _rect;

        void Awake() => _rect = GetComponent<RectTransform>();

        void Update()
        {
            if (Mouse.current == null) return;

            HandleZoom();
            HandlePan();
        }

        private void HandleZoom()
        {
            // Mouse Wheel을 이용한 Zooming
            float scrollY = Mouse.current.scroll.ReadValue().y;
            if (Mathf.Abs(scrollY) > 0.01f)
            {
                float zoomDelta = scrollY * zoomSpeed * 0.01f;
                Vector3 nextScale = _rect.localScale + Vector3.one * zoomDelta;

                nextScale.x = Mathf.Clamp(nextScale.x, minZoom, maxZoom);
                nextScale.y = Mathf.Clamp(nextScale.y, minZoom, maxZoom);
                _rect.localScale = nextScale;
            }
        }

        private void HandlePan()
        {
            // 이동 조건: 마우스 휠 클릭 OR Alt + 좌클릭 OR 마우스 우클릭
            bool isMiddlePanning = Mouse.current.middleButton.isPressed;
            bool isAltPanning = Keyboard.current.leftAltKey.isPressed && Mouse.current.leftButton.isPressed;
            bool isRightPanning = Mouse.current.rightButton.isPressed;

            if (isMiddlePanning || isAltPanning || isRightPanning)
            {
                // 마우스의 움직임 변화량(Delta)을 가져와서 위치에 더함
                Vector2 mouseDelta = Mouse.current.delta.ReadValue();

                // 화면의 스케일(Zoom)에 상관없이 일정한 속도로 움직이게 하려면 
                // 아래와 같이 scale 값을 나누어 계산할 수 있습니다.
                _rect.anchoredPosition += mouseDelta * (panSpeed / _rect.localScale.x);
            }
        }
    }
}