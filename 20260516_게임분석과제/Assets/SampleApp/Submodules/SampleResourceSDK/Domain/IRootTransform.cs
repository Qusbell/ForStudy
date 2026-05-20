using UnityEngine;

namespace SampleResourceSDK.Domain
{
    /// <summary>
    /// Transform 발사대
    /// </summary>
    public interface IRootTransform
    {
        /// <summary>
        /// Parent까지 다 거슬러 올라가서 받으란 뜻인가?
        /// </summary>
        Transform Transform { get; }
    }
}
