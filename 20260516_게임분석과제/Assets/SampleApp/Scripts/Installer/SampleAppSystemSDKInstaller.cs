using SampleAppSystemSDK.Application;
using SampleAppSystemSDK.Domain;
using SampleAppSystemSDK.Infrastructure;
using Zenject;

namespace SampleAppSystemSDK.Installer
{
    public class SampleAppSystemSDKInstaller : MonoInstaller
    {
        public override void InstallBindings()
        {
            // Application
            Container
                .Bind<IFileSystemApllication>()
                    .To<FileSystemApllication>()
                    .AsSingle();
            Container
                .Bind<IParseApplication>()
                    .To<ParseApplication>()
                    .AsSingle();

            // Domain
            Container
                .Bind<IFileSystemDomain>()
                    .To<FileSystemInfrastructure>()
                    .AsSingle();
            Container
                .Bind<IParseDomain>()
                    .To<CSVParseInfrastructure>()
                    .AsSingle();
        }
    }
}
