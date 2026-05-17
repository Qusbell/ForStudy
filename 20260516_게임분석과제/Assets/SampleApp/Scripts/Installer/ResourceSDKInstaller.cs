using SampleResourceSDK.Application;
using SampleResourceSDK.Domain;
using SampleResourceSDK.Infrastructure;
using UnityEngine;
using Zenject;

namespace SampleApp.Installer
{
    public class ResourceSDKInstaller : MonoInstaller
    {
        [SerializeField] private RootTransform _rootTransform = null;

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
