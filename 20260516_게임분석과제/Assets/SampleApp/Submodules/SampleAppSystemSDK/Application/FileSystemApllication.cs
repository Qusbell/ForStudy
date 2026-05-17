using SampleAppSystemSDK.Domain;

namespace SampleAppSystemSDK.Application
{
    public class FileSystemApllication : IFileSystemApllication
    {
        private IFileSystemDomain _fileSystemDomain;

        public FileSystemApllication(IFileSystemDomain fileSystemDomain)
        {
            _fileSystemDomain = fileSystemDomain;
        }

        public bool CheckInitialize( string fileName )
        {
            if( _fileSystemDomain.GetIsExist( fileName ) == false )
            {
                _fileSystemDomain.MoveToLocalStorage( fileName );
            }
            return _fileSystemDomain.GetIsExist( fileName );
        }

        public bool GetIsExist( string fileName )
            => _fileSystemDomain.GetIsExist(fileName);

        public string GetStringFromBinary( string fileName )
            => _fileSystemDomain.GetStringFromBinary( fileName );
    }
}
