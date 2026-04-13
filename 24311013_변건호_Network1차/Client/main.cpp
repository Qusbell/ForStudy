// NetworkBase에서 이미 선언 중
// #include <winsock2.h>
// #include <windows.h>

#include "ClientBase.h"
#include "ClientManager.h"
#include <tchar.h>
#include <string>

// UI 컨트롤 ID 정의
#define IDC_EDIT_IP      101
#define IDC_EDIT_PORT    102
#define IDC_BTN_CONNECT  103
#define IDC_EDIT_NAME    104
#define IDC_EDIT_ID      105
#define IDC_EDIT_CHAT    106
#define IDC_BTN_SEND     107
#define IDC_BTN_EXIT     108
#define IDC_LIST_CHAT    109

// 전역 변수
HINSTANCE hInst;                                // 현재 인스턴스 핸들
LPCTSTR szWindowClass = _T("ChattingAppClass"); // 창 클래스 이름
LPCTSTR szTitle = _T("Win API Network Chatting"); // 창 제목 바 이름

// 윈도우 창 요소들 (전역으로 관리하여 접근 용이성 확보)
HWND hEditIp, hEditPort, hBtnConnect;
HWND hEditName, hEditId;
HWND hEditChat, hBtnSend, hBtnExit;
HWND hListBox;

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
    wcex.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1); // 윈도우 기본 회색 배경
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

    if (!RegisterClassEx(&wcex))
    {
        return 1;
    }

    // B. 윈도우 생성 (크기 고정 스타일 적용)
    HWND hWnd = CreateWindow(
        szWindowClass,
        szTitle,
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, // 창 크기 조절 불가
        CW_USEDEFAULT, CW_USEDEFAULT, 480, 720,
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

    // D. 클라이언트 매니저 초기화 
    // (※ 기존에는 여기서 바로 TryStart를 했지만, UI 구성 후 Connect 버튼 이벤트에서 처리하도록 변경)
    ClientManager clientManager(hWnd);
    SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)&clientManager);

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
        // 폰트 설정용
        HFONT hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);

        // --- Row 1 : IP / Port / Connect ---
        CreateWindow(_T("STATIC"), _T("IP / Port"), WS_CHILD | WS_VISIBLE | SS_RIGHT | SS_CENTERIMAGE, 10, 10, 80, 25, hWnd, NULL, hInst, NULL);
        hEditIp = CreateWindowEx(WS_EX_CLIENTEDGE, _T("EDIT"), _T("127.0.0.1"), WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, 100, 10, 150, 25, hWnd, (HMENU)IDC_EDIT_IP, hInst, NULL);
        hEditPort = CreateWindowEx(WS_EX_CLIENTEDGE, _T("EDIT"), _T("9000"), WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, 260, 10, 80, 25, hWnd, (HMENU)IDC_EDIT_PORT, hInst, NULL);
        hBtnConnect = CreateWindow(_T("BUTTON"), _T("connect"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 350, 10, 100, 25, hWnd, (HMENU)IDC_BTN_CONNECT, hInst, NULL);

        // --- Row 2 : Name ---
        CreateWindow(_T("STATIC"), _T("name"), WS_CHILD | WS_VISIBLE | SS_RIGHT | SS_CENTERIMAGE, 10, 45, 80, 25, hWnd, NULL, hInst, NULL);
        hEditName = CreateWindowEx(WS_EX_CLIENTEDGE, _T("EDIT"), _T("Heesung Oh"), WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, 100, 45, 240, 25, hWnd, (HMENU)IDC_EDIT_NAME, hInst, NULL);

        // --- Row 3 : ID (Read Only) ---
        CreateWindow(_T("STATIC"), _T("ID"), WS_CHILD | WS_VISIBLE | SS_RIGHT | SS_CENTERIMAGE, 10, 80, 80, 25, hWnd, NULL, hInst, NULL);
        hEditId = CreateWindowEx(WS_EX_CLIENTEDGE, _T("EDIT"), _T(""), WS_CHILD | WS_VISIBLE | WS_BORDER | ES_READONLY | ES_AUTOHSCROLL, 100, 80, 240, 25, hWnd, (HMENU)IDC_EDIT_ID, hInst, NULL);

        // --- Row 4 : Chat Input ---
        CreateWindow(_T("STATIC"), _T("chat"), WS_CHILD | WS_VISIBLE | SS_RIGHT | SS_CENTERIMAGE, 10, 115, 80, 25, hWnd, NULL, hInst, NULL);
        hEditChat = CreateWindowEx(WS_EX_CLIENTEDGE, _T("EDIT"), _T(""), WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, 100, 115, 240, 25, hWnd, (HMENU)IDC_EDIT_CHAT, hInst, NULL);
        hBtnSend = CreateWindow(_T("BUTTON"), _T("send"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 350, 115, 100, 25, hWnd, (HMENU)IDC_BTN_SEND, hInst, NULL);

        // --- Row 5 : Exit Button ---
        hBtnExit = CreateWindow(_T("BUTTON"), _T("종료"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 350, 150, 100, 25, hWnd, (HMENU)IDC_BTN_EXIT, hInst, NULL);

        // --- Bottom : Chat ListBox ---
        hListBox = CreateWindowEx(WS_EX_CLIENTEDGE, _T("LISTBOX"), NULL, WS_CHILD | WS_VISIBLE | WS_VSCROLL | LBS_NOTIFY, 10, 185, 440, 480, hWnd, (HMENU)IDC_LIST_CHAT, hInst, NULL);

        // 생성된 모든 하위 컨트롤에 깔끔한 기본 GUI 폰트 적용
        EnumChildWindows(hWnd, [](HWND hwnd, LPARAM lParam) -> BOOL {
            SendMessage(hwnd, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);
            return TRUE;
            }, 0);
    }
    break;

    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        ClientManager* pMgr = (ClientManager*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

        switch (wmId)
        {
        case IDC_BTN_CONNECT:
        {
            if (pMgr == nullptr) break;

            // 1. IP 읽기
            int ipLen = GetWindowTextLengthA(hEditIp);
            std::string ipStr(ipLen, '\0');
            GetWindowTextA(hEditIp, &ipStr[0], ipLen + 1);

            // 2. Port 읽기
            int portLen = GetWindowTextLengthA(hEditPort);
            std::string portStr(portLen, '\0');
            GetWindowTextA(hEditPort, &portStr[0], portLen + 1);
            unsigned short port = (unsigned short)atoi(portStr.c_str());

            // 3. Name 읽기
            int nameLen = GetWindowTextLengthA(hEditName);
            std::string nameStr(nameLen, '\0');
            GetWindowTextA(hEditName, &nameStr[0], nameLen + 1);

            // 4. 서버 연결 시도 및 이름 전송
            if (pMgr->TryStart(ipStr, port, nameStr) == NetInitResult::Complete)
            {
                // 접속 완료 시 UI 버튼 및 입력창 비활성화 (중복 접속 방지)
                EnableWindow(hBtnConnect, FALSE);
                EnableWindow(hEditIp, FALSE);
                EnableWindow(hEditPort, FALSE);
                EnableWindow(hEditName, FALSE);

                SendMessageA(hListBox, LB_ADDSTRING, 0, (LPARAM)"[System] 서버에 연결되었습니다.");
            }
            else
            {
                MessageBox(hWnd, _T("서버 연결에 실패했습니다."), _T("Error"), MB_ICONERROR);
            }
            break;
        }

        case IDC_BTN_SEND:
        {
            // 전송 버튼 ID
            int len = GetWindowTextLengthA(hEditChat);
            if (len > 0 && pMgr)
            {
                std::string sendBuffer(len, '\0');
                GetWindowTextA(hEditChat, &sendBuffer[0], len + 1);

                pMgr->TrySendChat(sendBuffer);
                SetWindowText(hEditChat, _T("")); // 입력창 비우기
            }
            break;
        }

        case IDC_BTN_EXIT:
        {
            // 종료 버튼
            PostMessage(hWnd, WM_CLOSE, 0, 0);
            break;
        }
        }
        break;
    }

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

            // 제일 아래로 스크롤
            int count = SendMessage(hListBox, LB_GETCOUNT, 0, 0);
            SendMessage(hListBox, LB_SETCURSEL, count - 1, 0);
            SendMessage(hListBox, LB_SETCURSEL, -1, 0);
        }
    }
    break;

    // 서버로부터 ID를 부여받았을 때의 처리
    case WM_RECV_ID:
    {
        int assignedId = (int)wParam;
        char idStr[64];

        // 요구사항 이미지와 동일하게 0x 형식의 16진수 문자열로 표기
        sprintf_s(idStr, "0x%08X", assignedId);
        SetWindowTextA(hEditId, idStr);
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