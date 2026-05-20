using SampleResourceSDK.Application;
using SampleResourceSDK.Domain;
using SampleResourceSDK.Infrastructure;
using UnityEngine;
using Zenject;

namespace SampleApp.Installer
{
    /// <summary>
    /// Zenject라는 놈은 기본적으로 다음과 같이 동작한다: <br/>
    /// 1. 프로젝트 어딘가에 MonoInstaller를 상속받은 '물류 저장고(Container)'가 존재한다. (현재 파일)
    /// 2. '물류 저장고'에서는 '특정 딱지(Type, Key)'에 따른 '실제 물건(Instance, Value)'을 등록한다.
    /// 3. '물류 저장고'는 [Inject]가 붙은 메서드를 Awake 이전에 찾아서, '특정 딱지(Type, Key)'에 따른 '실제 물건(Instance, Value)'을 주입한다.
    /// 4. 따라서 [Inject]를 붙인 측에서는 '뭐가 됐든 틀(Type)에만 맞으면 물건(Instance)를 갖다줘라'고 선언하는 것이 된다.
    /// </summary>
    public class ResourceSDKInstaller : MonoInstaller
    {
        [SerializeField] private RootTransform _rootTransform = null;

        /// <summary>
        /// (아마도) Awake 이전 실행되어, '특정 딱지(Type, Key)'에 따른 '실제 물건(Instance, Value)'을 등록하는 메서드.
        /// </summary>
        public override void InstallBindings()
        {
            // Application
            Container
                .Bind<IResourceLoadApplication>()
                    .To<ResourceLoadApplication>()
                    .AsSingle();
            Container
                .Bind<IResourceDataListApplication>()
                    .To<ResourceDataListApplication>()
                    .AsSingle();
            Container
                .Bind<IBundleResourceApplication>()
                    .To<BundleResourceApplication>()
                    .AsSingle();

            // Domain
            Container
                .Bind<IResourceDataListDomain>()
                    .To<ResourceDataListDomain>()
                    .AsSingle();
            Container
                .Bind<IBundleResourceDomain>()
                    .To<StreamingAssetBundleLoader>()
                    .AsSingle();
            Container
                .Bind<IResourceFactoryDomain>()
                    .To<ResourceFactory>()
                    .AsSingle();
            Container
                .Bind<IRootTransform>()
                    .FromInstance( _rootTransform );
        }
    }
}
