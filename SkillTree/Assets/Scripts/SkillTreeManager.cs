using System;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;

namespace PoE_SkillTree
{
    /// <summary>
    /// 스킬트리의 논리적 상태와 포인트, 저장을 관리하는 핵심 매니저 클래스
    /// </summary>
    public class SkillTreeManager : MonoBehaviour
    {
        public static SkillTreeManager Instance { get; private set; }

        [Header("설정")]
        [SerializeField] private List<SkillNodeData> allNodes = new List<SkillNodeData>();
        [SerializeField] private int availablePoints = 20;

        private HashSet<string> _allocatedNodeIds = new HashSet<string>();
        // [추가] 런타임에서 양방향 연결을 관리하기 위한 맵
        private Dictionary<string, HashSet<string>> _adjacencyMap = new Dictionary<string, HashSet<string>>();

        public event Action OnTreeChanged;

        private void Awake()
        {
            if (Instance == null) Instance = this;
            else Destroy(gameObject);

            AutoLoadAllSkillAssets();
            BuildAdjacencyMap(); // [추가] 양방향 그래프 빌드
            LoadTree();
        }

        private void AutoLoadAllSkillAssets()
        {
            SkillNodeData[] loadedAssets = Resources.LoadAll<SkillNodeData>("SkillNodes");
            if (loadedAssets == null || loadedAssets.Length == 0)
                loadedAssets = Resources.LoadAll<SkillNodeData>("");

            allNodes = loadedAssets.ToList();
            Debug.Log($"<color=cyan>[SkillTreeManager]</color> 총 {allNodes.Count}개의 데이터를 로드했습니다.");
        }

        /// <summary>
        /// [개선] 에셋에 한쪽 방향으로만 연결되어 있어도 런타임에 양방향으로 연결합니다.
        /// </summary>
        private void BuildAdjacencyMap()
        {
            _adjacencyMap.Clear();
            foreach (var node in allNodes)
            {
                if (!_adjacencyMap.ContainsKey(node.id))
                    _adjacencyMap[node.id] = new HashSet<string>();

                foreach (var neighbor in node.neighbors)
                {
                    if (neighbor == null) continue;

                    // A -> B 연결
                    _adjacencyMap[node.id].Add(neighbor.id);

                    // B -> A 자동 역연결 (상호 참조 자동화)
                    if (!_adjacencyMap.ContainsKey(neighbor.id))
                        _adjacencyMap[neighbor.id] = new HashSet<string>();

                    _adjacencyMap[neighbor.id].Add(node.id);
                }
            }
            Debug.Log("<color=cyan>[SkillTreeManager]</color> 양방향 인접 맵 빌드 완료.");
        }

        public bool IsAllocated(string id) => _allocatedNodeIds.Contains(id);

        public bool CanAllocate(SkillNodeData node)
        {
            if (node == null || availablePoints <= 0 || IsAllocated(node.id)) return false;
            if (node.isStartNode) return true;

            // 인접 맵을 사용하여 연결된 노드가 있는지 확인
            if (_adjacencyMap.TryGetValue(node.id, out var neighbors))
            {
                return neighbors.Any(neighborId => IsAllocated(neighborId));
            }
            return false;
        }

        public void Allocate(SkillNodeData node)
        {
            if (!CanAllocate(node)) return;

            _allocatedNodeIds.Add(node.id);
            availablePoints--;

            Debug.Log($"<color=green>[SkillTree]</color> 할당: <b>{node.skillName}</b>. 남은 포인트: {availablePoints}");
            CalculateAndPrintStats();

            OnTreeChanged?.Invoke();
            SaveTree();
        }

        public void Deallocate(SkillNodeData node)
        {
            if (!IsAllocated(node.id) || node.isStartNode) return;

            _allocatedNodeIds.Remove(node.id);

            if (!IsConnectivityValid())
            {
                _allocatedNodeIds.Add(node.id);
                Debug.LogWarning("<color=red>[SkillTree] 해제 불가:</color> 경로가 끊어집니다.");
                return;
            }

            availablePoints++;
            Debug.Log($"<color=orange>[SkillTree]</color> 해제: <b>{node.skillName}</b>. 남은 포인트: {availablePoints}");
            CalculateAndPrintStats();

            OnTreeChanged?.Invoke();
            SaveTree();
        }

        private bool IsConnectivityValid()
        {
            if (_allocatedNodeIds.Count == 0) return true;

            var startNodes = allNodes.Where(n => n.isStartNode && IsAllocated(n.id)).ToList();
            if (startNodes.Count == 0 && _allocatedNodeIds.Count > 0) return false;

            HashSet<string> reachable = new HashSet<string>();
            Queue<string> queue = new Queue<string>();

            foreach (var sn in startNodes)
            {
                reachable.Add(sn.id);
                queue.Enqueue(sn.id);
            }

            while (queue.Count > 0)
            {
                string currentId = queue.Dequeue();
                if (_adjacencyMap.TryGetValue(currentId, out var neighbors))
                {
                    foreach (var neighborId in neighbors)
                    {
                        if (IsAllocated(neighborId) && !reachable.Contains(neighborId))
                        {
                            reachable.Add(neighborId);
                            queue.Enqueue(neighborId);
                        }
                    }
                }
            }

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

            string log = "<b>[보너스 총합]</b> ";
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