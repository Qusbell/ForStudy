using Cysharp.Threading.Tasks;
using SampleAppSystemSDK.Application;
using SampleResourceSDK.Application;
using SampleResourceSDK.Domain;
using System;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;
using UniRx;

namespace SampleApp.Model
{
    public class ResourceLoadModel : IResourceLoadModel, IDisposable
    {
        private IResourceLoadApplication _resourceLoadApplication;
        private IResourceDataListApplication _resourceDataListApplication;
        private IFileSystemApllication _fileSystemApplication;
        private IParseApplication _parseApplication;
        private IBundleResourceApplication _bundleResourceApplication;

        public IObservable<IReadOnlyList<ResourceDataInfo>> OnChangedList { get; }
        private CompositeDisposable _disposable = new CompositeDisposable();

        private readonly string ResourceCsvTablePath = "ResourceTable.csv";

        private List<string> _resourceNames = new List<string>();
        private Subject<IReadOnlyList<string>> _onResourceListChanged = new Subject<IReadOnlyList<string>>();
        public IObservable<IReadOnlyList<string>> OnResourceListChanged => _onResourceListChanged;


        public ResourceLoadModel( IResourceLoadApplication resourceALoadpplication,
            IResourceDataListApplication resourceDataListApplication,
            IFileSystemApllication fileSystemApplication,
            IParseApplication parseApplication,
            IBundleResourceApplication bundleResourceApplication)
        {
            _resourceLoadApplication = resourceALoadpplication;
            _resourceDataListApplication = resourceDataListApplication;
            _fileSystemApplication = fileSystemApplication;
            _parseApplication = parseApplication;
            _bundleResourceApplication = bundleResourceApplication;

            _resourceDataListApplication.OnChangedList
                .Subscribe( arg =>
                {
                    _resourceNames.AddRange( arg.Select( item => item.name ) );
                    _onResourceListChanged.OnNext( _resourceNames );
                } )
                .AddTo( _disposable );
        }

        public async UniTask<bool> InitializeProcess()
        {
            if( _fileSystemApplication.CheckInitialize( ResourceCsvTablePath ) == false )
            {
                Debug.LogError( "InitializeProcess Failed :: file" );
                return false;
            }
            var rawData = _fileSystemApplication.GetStringFromBinary( ResourceCsvTablePath );
            var list = _parseApplication.ParsingProcess<ResourceDataInfo>(rawData);
            if( list == null || list.Any() == false )
            {
                Debug.LogError( "InitializeProcess Failed :: list" );
                return false;
            }
            _resourceDataListApplication.AddIList( list );
            return true;
        }

        public async UniTask<bool> LoadResourceProcess( string id )
        {
            var isExist = _bundleResourceApplication.GetIsExist( id.ToLower() );
            Debug.Log( $"{id}, {isExist}" );
            var resource = await _bundleResourceApplication.LoadCharacterObject(id);
            return true;
        }

        public void Dispose()
        {
            _disposable.Dispose();
            _disposable.Clear();
        }
    }
}
