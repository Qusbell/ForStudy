using SampleResourceSDK.Domain;
using System;
using System.Collections.Generic;

namespace SampleResourceSDK.Application
{
    public class ResourceDataListApplication : IResourceDataListApplication
    {
        private IResourceDataListDomain _domain;

        public IReadOnlyList<ResourceDataInfo> DataInfoList => _domain.DataInfoList;

        public IObservable<IReadOnlyList<ResourceDataInfo>> OnChangedList => _domain.OnChangedList;

        public ResourceDataListApplication( IResourceDataListDomain domain)
        {
            _domain = domain;
        }

        public bool AddIList( IReadOnlyList<ResourceDataInfo> list )
            => _domain.AddIList( list );

        public bool AddItem( ResourceDataInfo data )
            => _domain.AddItem( data );

        public bool RemoveItem( ResourceDataInfo data )
            => _domain.RemoveItem( data );
    }
}
