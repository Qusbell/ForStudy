using System.Collections.Generic;

namespace SampleAppSystemSDK.Domain
{
    public interface IParseDomain
    {
        IReadOnlyList<T> ParsingProcess<T>( string rawData ) where T : new();
    }
}