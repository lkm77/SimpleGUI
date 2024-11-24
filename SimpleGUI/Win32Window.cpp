#include "Win32Window.h"

namespace SimpleGUI
{
    void Win32Window::CreateWin32Window()
    {
        Rect windowRect = AdjustWindow(wc);
        CreateWindowEx
        (
            wc.style.exStyle,
            wc.wndClass.lpszClassName,
            wc.name.c_str(),
            wc.style.style,
            windowRect.left, windowRect.top,
            windowRect.Width(), windowRect.Height(),
            wc.parent, wc.menu,
            hinstance, this
        );
        if (!hwnd)
        {
            hwnd = nullptr;
            isCreating=false;
            return;
        }
        ShowWindow(hwnd, SW_SHOW);//SW_SHOW:原样显示
        isCreating=false;

        messageLoop.Start(&Win32Window::ProcessMessages, this);
    }

    void Win32Window::Update()
    {
        if (windowManager)windowManager->OnUpdate(*this);
    }
    void Win32Window::ProcessMessages()
    {
        MSG msg;
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
            {
                hwnd = nullptr;
                messageLoop.Exit(false);
                return;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        Sleep(10);//减少CPU占用
    }

    LRESULT Win32Window::WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        switch (uMsg)
        {
        case WM_CREATE:
        {
            if (windowManager)windowManager->OnStart(*this);
            //窗口创建完成后,启动消息循环线程
            update.Start(&Win32Window::Update, this);
            break;
        }
        case WM_SIZE:
        {
            wc.clientRect.SetSize(LOWORD(lParam), HIWORD(lParam));
            if (windowManager)windowManager->OnResize(*this);
            break;
        }
        case WM_DESTROY:
            update.Close();

            if (windowManager)windowManager->OnDestroy(*this);

            PostQuitMessage(0);
            break;
        }
        //if (UserProcessMessages)return UserProcessMessages(*this, uMsg, wParam, lParam);
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
    

}