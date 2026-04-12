// NetworkBase에서 이미 선언 중
// #include <winsock2.h>
// #include <windows.h>

#include "ServerBase.h"
#include "ServerManager.h"
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


    // --- [여기서부터 테스트 코드 삽입] ---
    // 1. 서버 생성 (9000번 포트)

	ServerManager serverManager(DEFAULT_PORT);

    if (serverManager.TryStart() == NetInitResult::Complete)
    {
        MessageBoxA(hWnd, "Server started successfully!", "Network Test", MB_OK);
    }

    // --- [테스트 코드 끝] ---


    // C. 윈도우 표시
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

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