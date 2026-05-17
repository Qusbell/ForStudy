using Cysharp.Threading.Tasks;
using SampleCharacterSDK.Domain;

namespace SampleResourceSDK.Application
{
    /// <summary>
    /// 번들 리소스 애플리케이션 인터페이스 <br/>
    /// 현재는 캐릭터 관련 리소스 로드에 대해서만 정의된 것으로 보임 <br/>
    /// Character Resource Loader이란 이름이어도 되려나?? 싶음 <br/>
    /// UniTask(bool, ICharacter) 방식으로, Exist와 Object를 동시에 반환하는 방식은 어떨까요
    /// </summary>
    public interface IBundleResourceApplication
    {
        /// <summary>
        /// 대상 파일이 존재하는지 여부를 반환 <br/>
        /// 아마도 IsValid(유효성 검사)와 유사한 개념으로 생각됨
        /// </summary>
        /// <param name="fileName">대상 파일 이름</param>
        /// <returns>파일 존재 여부</returns>
        bool GetIsExist( string fileName );

        /// <summary>
        /// 캐릭터 오브젝트의 비동기 로딩
        /// </summary>
        /// <param name="fileName">대상 파일 이름</param>
        /// <returns>비동기 작업 결과로 반환되는 캐릭터 오브젝트</returns>
        UniTask<ICharacter> LoadCharacterObject( string fileName );

        //UniTask<(bool isExist, ICharacter character)> LoadExistAndCharacterObject(string fileName);
    }
}
