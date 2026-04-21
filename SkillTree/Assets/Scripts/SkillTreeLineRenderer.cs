using System.Collections.Generic;
using UnityEngine;

namespace PoE_SkillTree
{
    public class SkillTreeLineRenderer : MonoBehaviour
    {
        [Header("프리팹 설정")]
        [SerializeField] private GameObject linePrefab;
        [SerializeField] private Transform lineContainer;

        private void Start()
        {
            // Manager와 UI 배치가 끝난 뒤 생성하기 위해 약간의 지연을 줍니다.
            Invoke(nameof(GenerateLines), 0.1f);
        }

        public void GenerateLines()
        {
            if (linePrefab == null) return;

            SkillNodeUI[] allNodeUIs = GetComponentsInChildren<SkillNodeUI>();
            HashSet<string> createdPairs = new HashSet<string>();

            foreach (var uiA in allNodeUIs)
            {
                if (uiA.data == null) continue;

                foreach (var neighborData in uiA.data.neighbors)
                {
                    if (neighborData == null) continue;

                    SkillNodeUI uiB = System.Array.Find(allNodeUIs, x => x.data.id == neighborData.id);
                    if (uiB == null) continue;

                    string pairKey = uiA.data.id.CompareTo(uiB.data.id) < 0
                                     ? $"{uiA.data.id}_{uiB.data.id}"
                                     : $"{uiB.data.id}_{uiA.data.id}";

                    if (createdPairs.Contains(pairKey)) continue;

                    // 1. 오브젝트 생성
                    GameObject connectorObj = new GameObject($"Line_{pairKey}");

                    // 2. RectTransform 설정 (중요: 부모를 설정하기 전에 RectTransform을 먼저 확보)
                    RectTransform rect = connectorObj.AddComponent<RectTransform>();

                    // 3. 부모 설정
                    connectorObj.transform.SetParent(lineContainer != null ? lineContainer : transform);

                    // 4. [수정] 트랜스폼 초기화: 부모의 원점(0,0)에 정확히 맞춥니다.
                    rect.localPosition = Vector3.zero;
                    rect.localRotation = Quaternion.identity;
                    rect.localScale = Vector3.one;
                    rect.anchoredPosition = Vector2.zero; // 앵커 위치를 0,0으로

                    // 5. 커넥터 컴포넌트 추가 및 셋업
                    SkillTreeConnector connector = connectorObj.AddComponent<SkillTreeConnector>();
                    connector.Setup(uiA, uiB, linePrefab);

                    connectorObj.transform.SetAsFirstSibling();
                    createdPairs.Add(pairKey);
                }
            }
        }
    }
}