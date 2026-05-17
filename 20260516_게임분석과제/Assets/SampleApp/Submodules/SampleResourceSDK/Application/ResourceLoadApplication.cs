using System;

namespace SampleResourceSDK.Application
{
    public class ResourceLoadApplication : IResourceLoadApplication
    {
        public IObservable<bool> OnLoadComplete => throw new NotImplementedException();
    }
}
