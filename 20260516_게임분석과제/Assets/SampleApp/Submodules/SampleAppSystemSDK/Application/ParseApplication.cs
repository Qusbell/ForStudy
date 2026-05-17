using SampleAppSystemSDK.Domain;
using System.Collections.Generic;

namespace SampleAppSystemSDK.Application
{
    public class ParseApplication : IParseApplication
    {
        private IParseDomain _parseDomain;

        public ParseApplication(IParseDomain parseDomain)
        {
            _parseDomain = parseDomain;
        }

        public IReadOnlyList<T> ParsingProcess<T>( string rawData ) where T : new()
            => _parseDomain.ParsingProcess<T>( rawData );
    }
}
