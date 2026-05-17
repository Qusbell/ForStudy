using Cysharp.Threading.Tasks;
using SampleCharacterSDK.Domain;
using SampleResourceSDK.Domain;

namespace SampleResourceSDK.Application
{
    public class BundleResourceApplication : IBundleResourceApplication
    {
        private IBundleResourceDomain _bundleDomain;
        private IResourceFactoryDomain _factoryDomain;

        public BundleResourceApplication( IBundleResourceDomain bundleDomain, 
            IResourceFactoryDomain factoryDomain )
        {
            _bundleDomain = bundleDomain;
            _factoryDomain = factoryDomain;
        }

        public bool GetIsExist( string fileName )
            => _bundleDomain.GetIsExist( fileName );

        public async UniTask<ICharacter> LoadCharacterObject( string fileName )
        {
            var obj = await _bundleDomain.LoadObject( fileName );
            var character = await _factoryDomain.GenerateCharacter(obj);
            return character;
        }
    }
}
