#include <stdafx.h>

#include <Win.h>
#include <Ant.h>
#include <MathSimple.h>

InputQueue Win::m_inputQueue;

Win::Win(WinDesc p_desc) {
    m_desc = p_desc;
}
Win::~Win() {
}

HRESULT Win::init() {
    HRESULT hr = E_FAIL;

    // Extract desc:
    LPCTSTR title = m_desc.title;
    LPCTSTR className = m_desc.className;
    unsigned width = m_desc.width;
    unsigned height = m_desc.height;
    HINSTANCE hInstance = m_desc.hInstance;
    int nCmdShow = m_desc.nCmdShow;

    WNDCLASSEX wcex;
    ZERO_MEM(wcex);   // Use ZeroMemory to save some lines of code,
                      // and make up for it with a redundant comment.
    wcex.cbSize            = sizeof(WNDCLASSEX);
    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = Win::wWinProc;
    wcex.hInstance      = hInstance;
    wcex.hCursor        = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName   = NULL;
    wcex.lpszClassName  = className;

    LPTSTR lastMethod = "RegisterClassEx";
    if(RegisterClassEx(&wcex)!=0) {     // Registers a window class
                                        // for subsequent use in calls
                                        // to the CreateWindow or
                                        // CreateWindowEx function.
        RECT rct = { 0, 0, width, height };
        AdjustWindowRect(&rct, WS_OVERLAPPEDWINDOW, FALSE);

        lastMethod = "CreateWindow";
        m_hWnd = CreateWindow(
                     className,
                     title,
                     WS_OVERLAPPEDWINDOW,
                     CW_USEDEFAULT,
                     CW_USEDEFAULT,
                     rct.right    - rct.left,
                     rct.bottom    - rct.top,
                     NULL,
                     NULL,
                     hInstance,
                     NULL);
        if(m_hWnd!=NULL) {   // Return value of CreateWindow is NULL if failed.
            ShowWindow(m_hWnd, nCmdShow);
            //SetCapture( m_hWnd );
            hr = S_OK;
        }
    }
    if(hr!=S_OK) {
        Util::getLastErrorAndTerminateProcess(lastMethod);
    }
    return hr;
}

int Win::isWindowed() {
    return m_desc.isWindowed;
}
unsigned Win::getWidth() {
    RECT rct;
    GetClientRect(m_hWnd, &rct);
    return rct.right - rct.left;
}
unsigned Win::getHeight() {
    RECT rct;
    GetClientRect(m_hWnd, &rct);
    return rct.bottom - rct.top;
}
HWND Win::getHWnd() const {
    return m_hWnd;
}
InputQueue& Win::getInputQueue() {
    return m_inputQueue;
}

LRESULT CALLBACK Win::wWinProc(HWND hWnd, UINT message,
                               WPARAM wParam, LPARAM lParam) {
    HDC hdc;
    PAINTSTRUCT ps;
    LRESULT lResult = 0;
    if(Singleton<Ant>::get().eventWin(hWnd, message, wParam, lParam)==false) {
        // If message not handled by AntTweakBar
        switch(message) {
        case WM_PAINT:
            hdc = BeginPaint(hWnd, &ps);
            EndPaint(hWnd, &ps);
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        case WM_KEYDOWN:
            wWinKeyProc(wParam);
            break;
        case WM_MOUSEMOVE:
            wWinMouseProc(wParam, lParam);
            break;
        case WM_SIZE: // Sent when user resizes windows. Be sure to
                      // implement support for this at some point.
        case WM_LBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_MBUTTONUP:
        case WM_RBUTTONUP:
        default:
            lResult = DefWindowProc(hWnd, message, wParam, lParam);
            break;
        }
    }
    return lResult;
}
void Win::wWinKeyProc(WPARAM wParam) {
    InputKeyTypes keyType = InputKeyTypes_NA;
    switch(wParam) {
    case VK_ESCAPE:
        PostQuitMessage(0);
        break;

    case WinKeyW:
        keyType = InputKeyTypes_W;
        break;
    case WinKeyA:
        keyType = InputKeyTypes_A;
        break;
    case WinKeyS:
        keyType = InputKeyTypes_S;
        break;
    case WinKeyD:
        keyType = InputKeyTypes_D;
        break;

    case VK_UP:
        keyType = InputKeyTypes_UP;
        break;
    case VK_LEFT:
        keyType = InputKeyTypes_LEFT;
        break;
    case VK_DOWN:
        keyType = InputKeyTypes_DOWN;
        break;
    case VK_RIGHT:
        keyType = InputKeyTypes_RIGHT;
        break;

    case VK_SPACE:
        keyType = InputKeyTypes_SPACE;
        break;
    }
    if(keyType!=InputKeyTypes_NA) {
        m_inputQueue.keyPush(InputKey(keyType));
    }
}
void Win::wWinMouseProc(WPARAM wParam, LPARAM lParam) {
    float x = (float)GET_X_LPARAM(lParam);
    float y = (float)GET_Y_LPARAM(lParam);
    m_inputQueue.mousePush(InputMouse(x, y));
}

void Win::getLocalPos(double p_globalX, double p_globalY,
                      double& io_localX, double& io_localY) {
    RECT r;
    BOOL success = GetWindowRect(m_hWnd, &r);
    if(success==TRUE) {
        io_localX = clip<double>(p_globalX - r.left, 0, m_desc.width);
        io_localY = clip<double>(p_globalY - r.top, 0, m_desc.height);
        
    } else {
        throw ExceptionDv2520("Failed to retrieve the window rect. "
                              "Aborting...\n");
    }
}
