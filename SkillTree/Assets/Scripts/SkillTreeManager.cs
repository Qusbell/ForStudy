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
        [SerializeField] private int availablePoints = 20; // 기본값

        private HashSet<string> _allocatedNodeIds = new HashSet<string>();
        private Dictionary<string, HashSet<string>> _adjacencyMap = new Dictionary<string, HashSet<string>>();

        public event Action OnTreeChanged;

        private void Awake()
        {
            if (Instance == null) Instance = this;
            else Destroy(gameObject);

            AutoLoadAllSkillAssets();
            BuildAdjacencyMap();
            LoadTree(); // 로드 시 포인트와 할당 리스트를 모두 복원합니다.
        }

        private void AutoLoadAllSkillAssets()
        {
            SkillNodeData[] loadedAssets = Resources.LoadAll<SkillNodeData>("SkillNodes");
            if (loadedAssets == null || loadedAssets.Length == 0)
                loadedAssets = Resources.LoadAll<SkillNodeData>("");

            allNodes = loadedAssets.ToList();
            Debug.Log($"<color=cyan>[SkillTreeManager]</color> 총 {allNodes.Count}개의 데이터를 로드했습니다.");
        }

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
                    _adjacencyMap[node.id].Add(neighbor.id);

                    if (!_adjacencyMap.ContainsKey(neighbor.id))
                        _adjacencyMap[neighbor.id] = new HashSet<string>();

                    _adjacencyMap[neighbor.id].Add(node.id);
                }
            }
        }

        public bool IsAllocated(string id) => _allocatedNodeIds.Contains(id);

        public bool CanAllocate(SkillNodeData node)
        {
            if (node == null || availablePoints <= 0 || IsAllocated(node.id)) return false;
            if (node.isStartNode) return true;

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
            var data = new SkillTreeSaveData
            {
                availablePoints = this.availablePoints, // 현재 포인트 저장
                allocatedNodeIds = _allocatedNodeIds.ToList()
            };
            PlayerPrefs.SetString("SkillTree_Save", JsonUtility.ToJson(data));
            PlayerPrefs.Save();
        }

        public void LoadTree()
        {
            if (PlayerPrefs.HasKey("SkillTree_Save"))
            {
                var data = JsonUtility.FromJson<SkillTreeSaveData>(PlayerPrefs.GetString("SkillTree_Save"));
                this.availablePoints = data.availablePoints; // 저장된 포인트 복원
                _allocatedNodeIds = new HashSet<string>(data.allocatedNodeIds);
                OnTreeChanged?.Invoke();

                Debug.Log($"<color=cyan>[SkillTreeManager]</color> 데이터 로드 완료. (남은 포인트: {availablePoints})");
            }
        }
    }
}