namespace SampleAppSystemSDK.Application
{
    /// <summary>
    /// 파일 관련 기능을 제공하는 인터페이스
    /// </summary>
    public interface IFileSystemApllication
    {
        /// <summary>
        /// 바이너리 파일에서 문자열을 반환
        /// </summary>
        /// <param name="fileName">파일 이름</param>
        /// <returns>파일 내용 문자열</returns>
        string GetStringFromBinary( string fileName );

        /// <summary>
        /// 파일 초기화 여부를 확인
        /// </summary>
        /// <param name="fileName">파일 이름</param>
        /// <returns>초기화 여부</returns>
        bool CheckInitialize( string fileName );

        /// <summary>
        /// 파일 존재 여부를 확인
        /// </summary>
        /// <param name="fileName">파일 이름</param>
        /// <returns>존재 여부</returns>
        bool GetIsExist( string fileName );
    }
}
