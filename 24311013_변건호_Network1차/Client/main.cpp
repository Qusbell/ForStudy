// NetworkBase에서 이미 선언 중
// #include <winsock2.h>
// #include <windows.h>

#include "ClientBase.h"
#include <tchar.h>

// 전역 변수
HINSTANCE hInst;                                // 현재 인스턴스 핸들
LPCTSTR szWindowClass = _T("ChattingAppClass"); // 창 클래스 이름
LPCTSTR szTitle = _T("Win API Network Chatting"); // 창 제목 바 이름

// 함수의 선언 (Forward Declaration)
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// 1. WinMain: 프로그램의 시작점
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    hInst = hInstance;

    // A. 윈도우 클래스 등록
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc; // 메시지 처리 함수 연결
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

    if (!RegisterClassEx(&wcex))
    {
        return 1;
    }

    // B. 윈도우 생성
    HWND hWnd = CreateWindow(
        szWindowClass,
        szTitle,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 500, 700, // 크기: 가로 500, 세로 700
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (!hWnd)
    {
        return 0;
    }

    // C. 윈도우 표시
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);


    // --- [여기서부터 테스트 코드 삽입] ---
    // 1. 클라이언트 생성 (자기 자신 IP: 127.0.0.1)
    ClientBase client(DEFAULT_IP, DEFAULT_PORT);
    if (client.NetInitialize() == NetInitResult::Complete)
    {
        // 2. 부모(NetworkBase)의 소켓을 가져와 통신 객체 생성
        NetSignal signal(client.GetSocket());

        // 3. 데이터 수신 대기 (서버가 보낼 때까지 여기서 멈춤)
        std::string receivedMsg;
        if (signal.TryRecv(receivedMsg) > 0)
        {
            // 4. 수신 성공 시 메시지 박스 띄우기
            MessageBoxA(hWnd, receivedMsg.c_str(), "Network Test", MB_OK);
        }
    }
    // --- [테스트 코드 끝] ---


    // D. 메시지 루프: 프로그램이 종료될 때까지 메시지를 수신하고 전달
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

// 2. WndProc: 윈도우 메시지 처리기 (이벤트 핸들러)
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
        // 여기에 채팅창 UI (Edit Control, Button 등) 생성 코드가 들어갑니다.
        break;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        // 화면 그리기 로직 (필요 시)
        EndPaint(hWnd, &ps);
    }
    break;

    case WM_DESTROY:
        // 프로그램 종료 처리
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}