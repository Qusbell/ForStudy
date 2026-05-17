using SampleResourceSDK.Domain;
using System;
using System.Collections.Generic;

namespace SampleResourceSDK.Application
{
    /// <summary>
    /// 리소스 데이터 리스트 관리 <br/>
    /// IResourceDataListDomain과 완전히 같은 구성을 갖고 있으므로, 래핑 인터페이스로 추정됨
    /// </summary>
    public interface IResourceDataListApplication
    {
        /// <summary>
        /// 리소스 데이터 리스트
        /// </summary>
        public IReadOnlyList<ResourceDataInfo> DataInfoList { get; }

        /// <summary>
        /// 리소스 데이터가 변경될 때마다 발생하는 이벤트 <br/>
        /// 근데 이러면 DataInfoList를 get으로 노출시키는 이유가 있나?
        /// </summary>
        public IObservable<IReadOnlyList<ResourceDataInfo>> OnChangedList { get; }
        
        /// <summary>
        /// 리소스 데이터 추가
        /// </summary>
        /// <param name="data">추가할 리소스 데이터</param>
        /// <returns>추가 성공 여부</returns>
        bool AddItem( ResourceDataInfo data );

        /// <summary>
        /// 리소스 데이터 리스트 추가
        /// </summary>
        /// <param name="list">추가할 리소스 데이터 리스트</param>
        /// <returns>추가 성공 여부</returns>
        bool AddIList( IReadOnlyList<ResourceDataInfo> list );

        /// <summary>
        /// 리소스 데이터 제거
        /// </summary>
        /// <param name="data">제거할 리소스 데이터</param>
        /// <returns>제거 성공 여부</returns>
        bool RemoveItem( ResourceDataInfo data );
    }
}
