using System;
using System.Collections.Generic;
using System.Linq;
using UniRx;

namespace SampleResourceSDK.Domain
{
    public class ResourceDataListDomain : IResourceDataListDomain
    {
        private List<ResourceDataInfo> _list = new List<ResourceDataInfo>();

        public IReadOnlyList<ResourceDataInfo> DataInfoList => _list;

        private Subject<List<ResourceDataInfo>> _onChangedList = new Subject<List<ResourceDataInfo>>();
        public IObservable<IReadOnlyList<ResourceDataInfo>> OnChangedList => _onChangedList;

        public bool AddIList( IReadOnlyList<ResourceDataInfo> list )
        {
            _list.AddRange(list);
            _onChangedList.OnNext( _list );
            return true;
        }

        public bool AddItem( ResourceDataInfo data )
        {
            _list.Add( data );
            _onChangedList.OnNext( _list );
            return true;
        }

        public bool RemoveItem( ResourceDataInfo data )
        {
            var targetData = _list.FirstOrDefault(arg => string.Equals(arg.id, data.id));
            if( targetData == null )
            {
                return false;
            }
            _list.Remove( targetData );
            _onChangedList.OnNext( _list );
            return true;
        }
    }
}
