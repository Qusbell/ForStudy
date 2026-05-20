using Cysharp.Threading.Tasks;
using SampleCharacterSDK.Domain;

namespace SampleResourceSDK.Domain
{
    public interface IResourceFactoryDomain
    {
        /// <summary>
        /// (비동기) 캐릭터 생성
        /// </summary>
        /// <param name="obj">이거이 머이가(매개변수를 넣는 이유를 모르겠음)<br/>
        /// 구현체를 보니, 속에서 Object.Instantiate를 호출할 때 넣어줄 오브젝트인 거 같긴 함 (일종의 Object.Instantiate를 사용하기 위한 매개체?)
        /// 근데 그러면 구현체 쪽의 생성자같은 걸로 주입받는 게 낫지 않으까 싶기도 하고
        /// </param>
        /// <returns>생성된 캐릭터</returns>
        UniTask<ICharacter> GenerateCharacter( UnityEngine.Object obj ); 
    }
}
