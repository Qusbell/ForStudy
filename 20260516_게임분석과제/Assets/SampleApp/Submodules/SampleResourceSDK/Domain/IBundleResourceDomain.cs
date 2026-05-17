using Cysharp.Threading.Tasks;

namespace SampleResourceSDK.Domain
{
    public interface IBundleResourceDomain
    {
        bool GetIsExist( string fileName );
        UniTask<UnityEngine.Object> LoadObject( string fileName );
    }
}
