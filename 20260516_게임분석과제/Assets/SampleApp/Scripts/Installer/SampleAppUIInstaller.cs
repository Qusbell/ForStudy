using SampleApp.Model;
using SampleApp.Presenter;
using SampleApp.View;
using UnityEngine;
using Zenject;

namespace SampleApp.Installer
{
    public class SampleAppUIInstaller : MonoInstaller
    {
        [SerializeField] private SampleAppUIView _sampleAppUIView;

        public override void InstallBindings()
        {
            BindView();
            BindModel();
        }

        private void BindView()
        {
            Container
                .Bind<ISampleAppUIView>()
                .FromInstance( _sampleAppUIView );
        }

        private void BindModel()
        {
            Container
                .Bind<IResourceLoadModel>()
                .To<ResourceLoadModel>()
                .AsSingle();
        }
    }
}
