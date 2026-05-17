using Cysharp.Threading.Tasks;
using SampleApp.Model;
using UnityEngine;
using Zenject;
using UniRx;

namespace SampleApp.Presenter
{
    /// <summary>
    /// 1. MonoBehaviour의 형태로 제작된 Presenter <br/>
    /// 2. Initialize 메서드로 View와 Model을 주입받음 <br/>
    /// --> Zenject 활용 <br/>
    /// Zenject라는 놈은 기본적으로 다음과 같이 동작한다: <br/>
    /// 2.1. 프로젝트 어딘가에 MonoInstaller를 상속받은 '물류 저장고(Container)'가 존재한다.
    /// 2.2. '물류 저장고'에서는 '특정 딱지(Type, Key)'에 따른 '실제 물건(Instance, Value)'을 등록한다.
    /// 2.3. '물류 저장고'는 [Inject]가 붙은 메서드를 Awake 이전에 찾아서, '특정 딱지(Type, Key)'에 따른 '실제 물건(Instance, Value)'을 주입한다.
    /// 2.4. 따라서 [Inject]를 붙인 측에서는 '뭐가 됐든 틀(Type)에만 맞으면 물건(Instance)를 갖다줘라'고 선언하는 것이 된다.
    /// </summary>
    public class ResourceSelectPresenter : MonoBehaviour
    {
        /// <summary>
        /// 연결된 View (선택)
        /// </summary>
        private ISampleAppUIView _view;

        /// <summary>
        /// 연결된 Model (데이터 및 로직)
        /// </summary>
        private IResourceLoadModel _resourceLoadModel;

        /// <summary>
        /// 초기화
        /// </summary>
        /// <param name="view">연결할 View</param>
        /// <param name="resourceLoadModel">연결할 Model</param>
        [Inject]
        public void Initialize( ISampleAppUIView view,
            IResourceLoadModel resourceLoadModel )
        {
            _view = view;
            _resourceLoadModel = resourceLoadModel;
        }

        private async void Awake()
        {
            SubscribeView();
            SubscribeModel();

            await _resourceLoadModel.InitializeProcess();
        }

        /// <summary>
        /// View 구독
        /// </summary>
        private async void SubscribeView()
        {
            // 이건 그냥 예시 디버그용 구독으로 보임
            _view.OnSelectIndexChange
                .Subscribe( arg => Debug.Log( arg ) )
                .AddTo( this );

            // UI상에서 선택된 Name이 변경될 때마다, 해당 Name에 따른 Resource를 Load하려는 것으로 보임
            _view.OnSelectNameChange
                .Subscribe( async id => await _resourceLoadModel.LoadResourceProcess( id ) )
                .AddTo( this );
        }

        /// <summary>
        /// Model 구독
        /// </summary>
        private void SubscribeModel()
        {
            // Resource가 변경될 때마다, 해당 Resource 목록을 View로 전달하려는 것으로 보임
            _resourceLoadModel.OnResourceListChanged
                .Subscribe( arg => _view.SetOptions( arg ) )
                .AddTo( this );
        }
    }
}
