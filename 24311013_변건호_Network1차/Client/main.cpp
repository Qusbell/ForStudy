// NetworkBase에서 이미 선언 중
// #include <winsock2.h>
// #include <windows.h>

#include "ClientBase.h"
#include "ClientManager.h"
#include <tchar.h>

// 전역 변수
HINSTANCE hInst;                                // 현재 인스턴스 핸들
LPCTSTR szWindowClass = _T("ChattingAppClass"); // 창 클래스 이름
LPCTSTR szTitle = _T("Win API Network Chatting"); // 창 제목 바 이름

// 윈도우 창 요소들
HWND hListBox, hEdit, hButton;

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

	// D. 클라이언트 매니저 초기화 및 서버 연결 시도
    ClientManager clientManager(hWnd);
    SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)&clientManager);

    if (clientManager.TryStart(DEFAULT_IP, DEFAULT_PORT) == NetInitResult::Complete)
    {
        // 성공 시의 처리
    }
    else
    {
        // 실패 시의 처리
    }

    // E. 메시지 루프: 프로그램이 종료될 때까지 메시지를 수신하고 전달
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
    {
        // 1. 채팅 내역 출력창 (ListBox)
        hListBox = CreateWindowEx(WS_EX_CLIENTEDGE, _T("LISTBOX"), NULL,
            WS_CHILD | WS_VISIBLE | WS_VSCROLL | LBS_NOTIFY,
            10, 10, 460, 580, hWnd, (HMENU)1, hInst, NULL);

        // 2. 메시지 입력창 (Edit)
        hEdit = CreateWindowEx(WS_EX_CLIENTEDGE, _T("EDIT"), NULL,
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
            10, 600, 380, 30, hWnd, (HMENU)2, hInst, NULL);

        // 3. 전송 버튼 (Button)
        hButton = CreateWindow(_T("BUTTON"), _T("전송"),
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            400, 600, 70, 30, hWnd, (HMENU)3, hInst, NULL);
    }
    break;


//    case WM_COMMAND:
//        // 버튼 클릭 이벤트 (ID가 3인 버튼)
//        if (LOWORD(wParam) == 3)
//        {
//            TCHAR szInput[512];
//            GetWindowText(hEdit, szInput, 512); // 입력창 텍스트 가져오기
//
//            if (_tcslen(szInput) > 0)
//            {
//                // 저장해둔 매니저 포인터 꺼내기
//                ClientManager* pMgr = (ClientManager*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
//
//                // TCHAR를 std::string으로 변환하여 전송 (프로젝트 설정이 Unicode인 경우 고려)
//#ifdef UNICODE
//                std::wstring wstr(szInput);
//                std::string str(wstr.begin(), wstr.end());
//                pMgr->TrySendMessage(str);
//#else
//                pMgr->TrySendMessage(szInput);
//#endif
//                SetWindowText(hEdit, _T("")); // 입력창 비우기
//            }
//        }
//        break;

    case WM_COMMAND:
        if (LOWORD(wParam) == 3) // 전송 버튼 ID
        {
            // 1. 필요한 글자 수를 먼저 파악 (ANSI 버전 함수 호출)
            int len = GetWindowTextLengthA(hEdit);
            if (len > 0)
            {
                // 2. std::string에 공간을 미리 확보
                std::string sendBuffer(len, '\0');

                // 3. std::string 내부 메모리에 직접 ANSI 문자열을 채움
                // &sendBuffer[0]을 통해 내부 힙 메모리 주소에 직접 접근
                GetWindowTextA(hEdit, &sendBuffer[0], len + 1);

                // 4. 전송
                ClientManager* pMgr = (ClientManager*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
                pMgr->TrySendChat(sendBuffer);

                SetWindowText(hEdit, _T("")); // 입력창 비우기
            }
        }
        break;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        // 화면 그리기 로직 (필요 시)
        EndPaint(hWnd, &ps);
    }
    break;


    // 서버로부터 데이터가 수신되었을 때의 처리
    case WM_RECV_DATA:
    {
        ClientManager* clientManager = reinterpret_cast<ClientManager*>(lParam);
        if (clientManager != nullptr)
        {
            std::string message = clientManager->GetRecvMessage();
            SendMessageA(hListBox, LB_ADDSTRING, 0, (LPARAM)message.c_str());
        }
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