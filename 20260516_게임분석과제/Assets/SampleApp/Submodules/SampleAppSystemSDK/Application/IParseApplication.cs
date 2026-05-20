using System.Collections.Generic;

namespace SampleAppSystemSDK.Application
{
    /// <summary>
    /// 대상 string로부터 원하는 다른 데이터를 Parsing하는 인터페이스
    /// </summary>
    public interface IParseApplication
    {
        /// <summary>
        /// 대상 string을파싱하여 원하는 형식의 데이터를 반환
        /// </summary>
        /// <typeparam name="T">원하는 데이터 Type</typeparam>
        /// <param name="rawData">대상 데이터 (Key)</param>
        /// <returns>파싱된 데이터 리스트</returns>
        IReadOnlyList<T> ParsingProcess<T>( string rawData ) where T : new();
    }
}
