using Cysharp.Threading.Tasks;
using SampleResourceSDK.Domain;
using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace SampleApp.Model
{
    /// <summary>
    /// 리소스 로드 규칙 <br/>
    /// </summary>
    public interface IResourceLoadModel
    {
        /// <summary>
        /// 리소스 목록이 변경될 때 발생하는 이벤트
        /// </summary>
        IObservable<IReadOnlyList<string>> OnResourceListChanged { get; }

        /// <summary>
        /// 최초 리소스 목록을 가져옴 (초기화)
        /// </summary>
        /// <returns>지연된 성공 여부</returns>
        UniTask<bool> InitializeProcess();

        /// <summary>
        /// 특정 리소스를 로드
        /// </summary>
        /// <param name="id">로드할 리소스의 ID</param>
        /// <returns>지연된 성공 여부</returns>
        UniTask<bool> LoadResourceProcess( string id );
    }
}
