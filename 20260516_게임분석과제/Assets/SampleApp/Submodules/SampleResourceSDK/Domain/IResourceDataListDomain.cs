using System;
using System.Collections.Generic;

namespace SampleResourceSDK.Domain
{
    /// <summary>
    /// IResourceDataListApplication와 완전히 동일한 구성 <br/>
    /// 여기서는 주석 생략
    /// </summary>
    public interface IResourceDataListDomain
    {
        public IReadOnlyList<ResourceDataInfo> DataInfoList { get; }
        public IObservable<IReadOnlyList<ResourceDataInfo>> OnChangedList { get; }
        bool AddIList( IReadOnlyList<ResourceDataInfo> list );
        bool AddItem( ResourceDataInfo data );
        bool RemoveItem( ResourceDataInfo data );
    }
}
