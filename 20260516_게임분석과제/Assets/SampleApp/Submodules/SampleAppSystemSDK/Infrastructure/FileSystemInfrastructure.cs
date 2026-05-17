using SampleAppSystemSDK.Domain;
using System.IO;

namespace SampleAppSystemSDK.Infrastructure
{
    public class FileSystemInfrastructure : IFileSystemDomain
    {
        private readonly string RootPath = UnityEngine.Application.streamingAssetsPath;
        private readonly string OriginPath = UnityEngine.Application.dataPath;

        public string GetStringFromBinary( string fileName )
        {
            var path = Path.Combine(RootPath, fileName);
            using( var fs = new FileStream( path, FileMode.Open ) )
            using( var sr = new StreamReader( fs ) )
            {
                var data = sr.ReadToEnd();
                return data;
            }
        }

        public bool GetIsExist( string fileName )
        {
            var path = Path.Combine(RootPath, fileName);
            return File.Exists( path );
        }

        public bool MoveToLocalStorage( string fileName )
        {
            var originPath = Path.Combine(OriginPath, fileName);
            var destPath = Path.Combine(RootPath, fileName);
            if(Directory.Exists(originPath) == false)
            {
                var dirName = Path.GetDirectoryName( destPath );
                Directory.CreateDirectory( dirName );
            }
            File.Move( originPath, destPath );

            return GetIsExist( fileName );
        }
    }
}
