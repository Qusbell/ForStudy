using Cysharp.Threading.Tasks;
using SampleEditorSDK.Domain;
using UnityEditor;
using System.Collections.Generic;
using UnityEngine;
using System.Linq;
using SampleCharacterSDK.Domain;
using System.IO;
using System;

namespace SampleEditorSDK.View
{
    public class StanaloneAssetBundleBuilder : IAssetBundleBuildDomain
    {
        private List<string> _guids = new List<string>();
        private List<AssetBundleBuild> _buildMap = new List<AssetBundleBuild>();
        private const string TargetFolder = "Assets/SampleApp/Submodules/SampleCharacterSDK/AssetBundles/";

        public async UniTask<bool> PreProcess( BuildTargetGroup platform )
        {
            try
            {
                _guids = AssetDatabase.FindAssets( "t:Prefab", new[] { TargetFolder } ).ToList();

                foreach(var guid in _guids)
                {
                    var assetPath = AssetDatabase.GUIDToAssetPath(guid);
                    var prefab = AssetDatabase.LoadAssetAtPath<GameObject>(assetPath);
                    Debug.Log( prefab.name );
                    if(prefab != null)
                    {
                        var targetComponent = prefab.GetComponent<ICharacter>();
                        if(targetComponent != null)
                        {
                            var build = new AssetBundleBuild();
                            build.assetBundleName = prefab.name + ".ab";
                            build.assetNames = new[] { assetPath };
                            _buildMap.Add( build );
                        }
                    }
                }

                if(_buildMap.Count == 0)
                {
                    Debug.LogError( "Can Not Find Exist Any Component Assets" );
                    return false;
                }
                Debug.Log( $"AssetBundle PreProcess Successed" );
                return true;
            }
            catch(Exception e)
            {
                Debug.LogError( $"AssetBundle PreProcess Failed: {e.Message}" );
                return false;
            }
        }

        public async UniTask<bool> BuildProcess( BuildTargetGroup platform )
        {
            try
            {
                var projectRoot = Directory.GetParent(UnityEngine.Application.dataPath).FullName;

                var buildDirectory = Path.Combine(projectRoot,
                "Builds",
                platform.ToString(),
                DateTime.Now.ToString("yyyyMMdd_HHmmss"));

                if(Directory.Exists( buildDirectory ) == false)
                {
                    Directory.CreateDirectory( buildDirectory );
                }

                var manifest = BuildPipeline.BuildAssetBundles(
                    buildDirectory,
                    _buildMap.ToArray(),
                    BuildAssetBundleOptions.None,
                    EditorUserBuildSettings.activeBuildTarget
                );

                if( manifest == null )
                {
                    Debug.LogError( "Manifest Build Failed" );
                    return false;
                }

                var manifestFilePath = Path.Combine(buildDirectory, "BundleManifestInfo.txt");
                using( var writer = new StreamWriter( manifestFilePath ) )
                {
                    string[] builtBundles = manifest.GetAllAssetBundles();
                    foreach(string bundleName in builtBundles)
                    {
                        var bundleHash = manifest.GetAssetBundleHash(bundleName).ToString();
                        writer.WriteLine( $"{bundleName}/{bundleHash}" );
                    }
                }
                Debug.Log( $"AssetBundle BuildProcess Successed: {_buildMap.Count} to `{buildDirectory}`" );
                return true;
            }
            catch( Exception e )
            {
                Debug.LogError( $"AssetBundle BuildProcess Failed: {e.Message}" );
                return false;
            }
        }

        public async UniTask<bool> PostProcess( BuildTargetGroup platform )
        {
            try
            {
                Debug.Log( $"AssetBundle PostProcess Successed" );
                return true;
            }
            catch(Exception e)
            {
                Debug.LogError( $"AssetBundle PostProcess Failed: {e.Message}" );
                return false;
            }
        }
    }
}
