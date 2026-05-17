using SampleResourceSDK.Domain;
using UnityEngine;

namespace SampleResourceSDK.Infrastructure
{
    public class RootTransform : MonoBehaviour, IRootTransform
    {
        [SerializeField] private Transform _transform;
        public Transform Transform => _transform;
    }
}
