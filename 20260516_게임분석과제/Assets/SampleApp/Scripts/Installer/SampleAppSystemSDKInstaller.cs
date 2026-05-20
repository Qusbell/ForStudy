using SampleAppSystemSDK.Application;
using SampleAppSystemSDK.Domain;
using SampleAppSystemSDK.Infrastructure;
using Zenject;

namespace SampleAppSystemSDK.Installer
{
    /// <summary>
    /// ResourceSDKInstaller와 동일 구조
    /// </summary>
    public class SampleAppSystemSDKInstaller : MonoInstaller
    {
        /// <summary>
        /// (아마도) Awake 이전 실행되어, '특정 딱지(Type, Key)'에 따른 '실제 물건(Instance, Value)'을 등록하는 메서드.
        /// </summary>
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
