using Cysharp.Threading.Tasks;
using System.Threading.Tasks;
using UnityEditor;
using UnityEngine;

namespace SampleEditorSDK.View
{
    public class AssetBundleBuildMenuItem : MonoBehaviour
    {
        private const string MENU_NAME_ASSETBUNDLE_ONLY = "SampleTool/AssetBundleBuild";
        [MenuItem( MENU_NAME_ASSETBUNDLE_ONLY, priority = 0 )]
        private static async UniTask<bool> AssetBundleBuildOnly()
        {
            var platform = EditorUserBuildSettings.activeBuildTarget.ToString();
            return await AssetBundleBuildView.ExecuteAssetBundleBuild( platform );
        }
    }
}
