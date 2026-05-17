using Cysharp.Threading.Tasks;
using System.Collections;
using System.Collections.Generic;
using UnityEditor;
using UnityEngine;

namespace SampleEditorSDK.Domain
{
    public interface IAssetBundleBuildDomain
    {
        UniTask<bool> PreProcess( BuildTargetGroup platform );
        UniTask<bool> BuildProcess( BuildTargetGroup platform );
        UniTask<bool> PostProcess( BuildTargetGroup platform );
    }
}
