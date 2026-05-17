using System;

namespace SampleResourceSDK.Application
{
    public interface IResourceLoadApplication
    {
        /// <summary>
        /// 로딩 완료 시
        /// </summary>
        IObservable<bool> OnLoadComplete { get; }
    }
}
