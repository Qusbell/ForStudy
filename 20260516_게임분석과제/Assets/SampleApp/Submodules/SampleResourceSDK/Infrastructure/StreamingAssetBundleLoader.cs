using Cysharp.Threading.Tasks;
using SampleResourceSDK.Domain;
using System.IO;
using UnityEngine;

namespace SampleResourceSDK.Infrastructure
{
    public class StreamingAssetBundleLoader : IBundleResourceDomain
    {
        private readonly string RootPath = Path.Combine(UnityEngine.Application.streamingAssetsPath, "AssetBundles");
        private readonly string Prefix = ".ab";
        
        public bool GetIsExist( string fileName )
        {
            var path = Path.Combine(RootPath, $"{fileName}{Prefix}");
            return File.Exists( path );
        }

        public async UniTask<Object> LoadObject( string fileName )
        {
            var path = Path.Combine(RootPath, $"{fileName}{Prefix}");
            if(!File.Exists( path ))
            {
                Debug.LogError( $"Not Exist File :: {path}" );
                return null;
            }

            var bundle = await AssetBundle.LoadFromFileAsync(path).ToUniTask();
            if(bundle == null)
            {
                Debug.LogError( $"Not Exist AssetBundle File :: {path}" );
                return null;
            }

            var loadedAsset = await bundle.LoadAssetAsync<Object>(fileName).ToUniTask();
            if(loadedAsset == null)
            {
                Debug.LogError( $"Not Exist Asset in AssetBundle File :: {fileName}" );
            }

            bundle.Unload( false );
            return loadedAsset;
        }
    }
}
