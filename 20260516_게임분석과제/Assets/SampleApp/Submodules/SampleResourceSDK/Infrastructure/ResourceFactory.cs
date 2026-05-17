using Cysharp.Threading.Tasks;
using SampleCharacterSDK.Domain;
using SampleResourceSDK.Domain;
using UnityEngine;

namespace SampleResourceSDK.Infrastructure
{
    public class ResourceFactory : IResourceFactoryDomain
    {
        private IRootTransform _rootTransform;

        public ResourceFactory( IRootTransform rootTransform )
        {
            _rootTransform = rootTransform;
        }

        public async UniTask<ICharacter> GenerateCharacter( Object obj )
        {
            var prefab = obj as GameObject;
            if(prefab == null)
            {
                Debug.LogError( $"{obj.name} Is Not GameObject Type" );
                return null;
            }

            var instance = _rootTransform != null
                ? Object.Instantiate( prefab, _rootTransform.Transform )
                : Object.Instantiate( prefab );

            instance.transform.localPosition = Vector3.zero;
            instance.transform.localRotation = Quaternion.identity;
            instance.transform.localScale = Vector3.one;

            var character = instance.GetComponent<ICharacter>();

            if(character == null)
            {
                Debug.LogError( $"Not Exist Character Component In Prefab" ); 
                Object.Destroy( instance );
                return null;
            }

            await UniTask.Yield();
            return character;
        }
    }
}
