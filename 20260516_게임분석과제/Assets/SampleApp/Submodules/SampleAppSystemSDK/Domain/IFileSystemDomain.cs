namespace SampleAppSystemSDK.Domain
{
    /// <summary>
    /// 대상 파일로부터 String의 형태로 데이터를 꺼내오는 인터페이스
    /// </summary>
    public interface IFileSystemDomain
    {
        /// <summary>
        /// 대상 파일로부터 string으로 반환
        /// </summary>
        /// <param name="fileName">대상 파일명</param>
        /// <returns>파싱된 데이터</returns>
        string GetStringFromBinary( string fileName );

        /// <summary>
        /// 이거 이 정도로 반복되면 그냥 IIsExist 인터페이스로 빼는 게 맞지 않나
        /// </summary>
        /// <param name="fileName">대상 파일명</param>
        /// <returns>파일 존재 여부</returns>
        bool GetIsExist( string fileName );

        /// <summary>
        /// 뭔지는 모르겠지만 뭔가 로컬 저장소로 옮긴다고 하는 것 같음 <br/>
        /// 구현체 상으로도 뭔가 파일을 조물딱거리고 있음
        /// </summary>
        /// <param name="fileName">대상 파일명</param>
        /// <returns>로컬 저장소로 이동 성공 여부?</returns>
        bool MoveToLocalStorage( string fileName );
    }
}
