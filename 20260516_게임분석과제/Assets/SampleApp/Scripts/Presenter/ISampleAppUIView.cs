using System;
using System.Collections.Generic;

namespace SampleApp.Presenter
{
    public interface ISampleAppUIView
    {
        /// <summary>
        /// 선택된 Index가 변경될 때마다 발생하는 이벤트
        /// </summary>
        IObservable<int> OnSelectIndexChange { get; }

        /// <summary>
        /// 선택된 Name이 변경될 때마다 발생하는 이벤트
        /// </summary>
        IObservable<string> OnSelectNameChange { get; }

        /// <summary>
        /// (아마도) UI상에서 선택 가능한 옵션 목록을 설정
        /// </summary>
        /// <param name="options">옵션 목록들</param>
        void SetOptions( IReadOnlyList<string> options );
    }
}
