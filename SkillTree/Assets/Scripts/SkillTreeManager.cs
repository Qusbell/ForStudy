using System;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;

namespace PoE_SkillTree
{
    /// <summary>
    /// 스킬트리의 논리적 상태와 포인트, 저장을 관리하는 핵심 매니저 클래스 (Logic Layer)
    /// </summary>
    public class SkillTreeManager : MonoBehaviour
    {
        public static SkillTreeManager Instance { get; private set; }

        [Header("설정")]
        [SerializeField] private List<SkillNodeData> allNodes = new List<SkillNodeData>();
        [SerializeField] private int availablePoints = 10;

        private HashSet<string> _allocatedNodeIds = new HashSet<string>();
        public event Action OnTreeChanged;

        private void Awake()
        {
            if (Instance == null) Instance = this;
            else Destroy(gameObject);

            // [추가] 프로젝트 내의 모든 스킬 데이터 에셋을 자동으로 수집
            AutoLoadAllSkillAssets();
            LoadTree();
        }

        /// <summary>
        /// Resources/SkillNodes 폴더 내의 모든 SkillNodeData 에셋을 자동으로 로드합니다.
        /// </summary>
        private void AutoLoadAllSkillAssets()
        {
            // 경로: Assets/Resources/SkillNodes 내의 모든 SkillNodeData 타입 에셋 로드
            SkillNodeData[] loadedAssets = Resources.LoadAll<SkillNodeData>("SkillNodes");

            if (loadedAssets == null || loadedAssets.Length == 0)
            {
                Debug.LogWarning("<color=yellow>[SkillTreeManager]</color> Resources/SkillNodes 폴더에서 스킬 데이터를 찾을 수 없습니다! 폴더 구조를 확인하세요.");
                // 폴더가 없는 경우 대비하여 Resources 루트에서도 탐색
                loadedAssets = Resources.LoadAll<SkillNodeData>("");
            }

            allNodes = loadedAssets.ToList();
            Debug.Log($"<color=cyan>[SkillTreeManager]</color> 총 {allNodes.Count}개의 스킬 데이터를 자동으로 로드했습니다.");
        }

        public bool IsAllocated(string id) => _allocatedNodeIds.Contains(id);

        public bool CanAllocate(SkillNodeData node)
        {
            if (availablePoints <= 0 || IsAllocated(node.id)) return false;
            if (node.isStartNode) return true;

            // 인접 노드 중 하나라도 할당되어 있는지 확인
            return node.neighbors.Any(neighbor => IsAllocated(neighbor.id));
        }

        public void Allocate(SkillNodeData node)
        {
            if (!CanAllocate(node)) return;

            _allocatedNodeIds.Add(node.id);
            availablePoints--;

            Debug.Log($"<color=cyan>[SkillTree]</color> 할당 성공: {node.skillName}. 남은 포인트: {availablePoints}");
            CalculateAndPrintStats();

            OnTreeChanged?.Invoke();
            SaveTree();
        }

        public void Deallocate(SkillNodeData node)
        {
            // 할당되지 않았거나 시작 노드인 경우 해제 불가
            if (!IsAllocated(node.id) || node.isStartNode) return;

            _allocatedNodeIds.Remove(node.id);

            // 경로 단절 검사 (BFS): 트리의 모든 활성 노드가 시작 노드와 연결되어 있는지 확인
            if (!IsConnectivityValid())
            {
                _allocatedNodeIds.Add(node.id); // 복구
                Debug.LogWarning("<color=red>[SkillTree] 할당 해제 불가:</color> 해당 노드를 제거하면 연결 경로가 끊어집니다.");
                return;
            }

            availablePoints++;
            Debug.Log($"<color=orange>[SkillTree]</color> 해제 성공: {node.skillName}. 남은 포인트: {availablePoints}");
            CalculateAndPrintStats();

            OnTreeChanged?.Invoke();
            SaveTree();
        }

        private bool IsConnectivityValid()
        {
            if (_allocatedNodeIds.Count == 0) return true;

            // 할당된 노드 중 시작 노드들을 찾음
            var startNodes = allNodes.Where(n => n.isStartNode && IsAllocated(n.id)).ToList();
            if (startNodes.Count == 0 && _allocatedNodeIds.Count > 0) return false;

            // BFS를 통해 도달 가능한 모든 할당된 노드 탐색
            HashSet<string> reachable = new HashSet<string>();
            Queue<SkillNodeData> queue = new Queue<SkillNodeData>(startNodes);
            foreach (var sn in startNodes) reachable.Add(sn.id);

            while (queue.Count > 0)
            {
                var current = queue.Dequeue();
                foreach (var neighbor in current.neighbors)
                {
                    if (IsAllocated(neighbor.id) && !reachable.Contains(neighbor.id))
                    {
                        reachable.Add(neighbor.id);
                        queue.Enqueue(neighbor);
                    }
                }
            }

            // 도달 가능한 노드 수와 전체 할당된 노드 수가 같아야 유효함
            return reachable.Count == _allocatedNodeIds.Count;
        }

        private void CalculateAndPrintStats()
        {
            Dictionary<StatType, float> totals = new Dictionary<StatType, float>();
            foreach (var id in _allocatedNodeIds)
            {
                var node = allNodes.Find(n => n.id == id);
                if (node == null) continue;
                foreach (var mod in node.modifiers)
                {
                    if (!totals.ContainsKey(mod.type)) totals[mod.type] = 0;
                    totals[mod.type] += mod.value;
                }
            }

            string log = "<b>[현재 보너스 총합]</b> ";
            foreach (var kvp in totals) log += $"{kvp.Key}(+{kvp.Value}) ";
            Debug.Log(log);
        }

        public void SaveTree()
        {
            var data = new SkillTreeSaveData { allocatedNodeIds = _allocatedNodeIds.ToList() };
            PlayerPrefs.SetString("SkillTree_Save", JsonUtility.ToJson(data));
            PlayerPrefs.Save();
        }

        public void LoadTree()
        {
            if (PlayerPrefs.HasKey("SkillTree_Save"))
            {
                var data = JsonUtility.FromJson<SkillTreeSaveData>(PlayerPrefs.GetString("SkillTree_Save"));
                _allocatedNodeIds = new HashSet<string>(data.allocatedNodeIds);
                OnTreeChanged?.Invoke();
            }
        }
    }
}