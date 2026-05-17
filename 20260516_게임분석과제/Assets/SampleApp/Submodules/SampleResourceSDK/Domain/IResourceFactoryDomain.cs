using Cysharp.Threading.Tasks;
using SampleCharacterSDK.Domain;

namespace SampleResourceSDK.Domain
{
    public interface IResourceFactoryDomain
    {
        UniTask<ICharacter> GenerateCharacter( UnityEngine.Object obj ); 
    }
}
