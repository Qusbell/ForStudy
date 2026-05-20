using Cysharp.Threading.Tasks;

namespace SampleResourceSDK.Domain
{
    public interface IBundleResourceDomain
    {
        /// <summary>
        /// 파일 존재 여부 반환
        /// </summary>
        /// <param name="fileName">대상 파일명</param>
        /// <returns>파일 존재 여부</returns>
        bool GetIsExist( string fileName );

        /// <summary>
        /// (비동기) 오브젝트 로드. 아마도 AssetBundle 관련.
        /// </summary>
        /// <param name="fileName">대상 파일명</param>
        /// <returns>로드된 오브젝트</returns>
        UniTask<UnityEngine.Object> LoadObject( string fileName );
    }
}
