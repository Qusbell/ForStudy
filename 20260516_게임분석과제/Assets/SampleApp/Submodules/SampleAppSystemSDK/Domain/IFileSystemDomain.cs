namespace SampleAppSystemSDK.Domain
{
    public interface IFileSystemDomain
    {
        string GetStringFromBinary( string fileName );
        bool GetIsExist( string fileName );
        bool MoveToLocalStorage( string fileName );
    }
}
