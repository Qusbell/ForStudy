using SampleApp.Model;
using SampleApp.Presenter;
using SampleApp.View;
using UnityEngine;
using Zenject;

namespace SampleApp.Installer
{
    /// <summary>
     /// ResourceSDKInstaller와 동일 구조
     /// </summary>
    public class SampleAppUIInstaller : MonoInstaller
    {
        [SerializeField] private SampleAppUIView _sampleAppUIView;

        /// <summary>
        /// (아마도) Awake 이전 실행되어, '특정 딱지(Type, Key)'에 따른 '실제 물건(Instance, Value)'을 등록하는 메서드.
        /// </summary>
        public override void InstallBindings()
        {
            BindView();
            BindModel();
        }

        /// <summary>
        /// 그냥 View 바인드 래핑 메서드
        /// </summary>
        private void BindView()
        {
            Container
                .Bind<ISampleAppUIView>()
                .FromInstance( _sampleAppUIView );
        }

        /// <summary>
        /// 마찬가지로 그냥 Model 바인드 래핑 메서드
        /// </summary>
        private void BindModel()
        {
            Container
                .Bind<IResourceLoadModel>()
                .To<ResourceLoadModel>()
                .AsSingle();
        }
    }
}
