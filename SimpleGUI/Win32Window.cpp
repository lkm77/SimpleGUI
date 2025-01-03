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
        {
            //为了避免多线程问题,Update与Resize函数在同一线程中执行
            //{}为了限制true_const范围
            bool true_const = true;//禁止修改,仅用于compare_exchange_strong
            if (resizeFlag.compare_exchange_strong(true_const, false))
            {
                bitmap.SetSize(wc.clientRect.Width(), wc.clientRect.Height());
                callbackFunction.ProcessingModifications();
                if (callbackFunction.Get().OnResize)
                {
                    callbackFunction.Get().OnResize(*this);
                }
            }
        }
        if(updateFrameRateController.IsNextFrame())
        {
            callbackFunction.ProcessingModifications();
            if (callbackFunction.Get().Update)
            {
                callbackFunction.Get().Update(*this);
            }
        }
        bitmap.DrawToHwnd(hwnd);
        Sleep(10);//减少CPU占用
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
            //消息循环线程未开启,不会导致多线程问题
            if (callbackFunction.Get().Start)callbackFunction.Get().Start(*this);
            //窗口创建完成后,启动消息循环线程
            update.Start(&Win32Window::Update, this);
            break;
        }
        case WM_MOVING:
        {
            RECT* rect = (RECT*)lParam;
            wc.clientRect.SetPosition(rect->left, rect->top);
            break;
        }
        case WM_SIZE:
        {
            wc.clientRect.SetSize(LOWORD(lParam), HIWORD(lParam));
            resizeFlag.store(true);
            break;
        }
        case WM_DESTROY:
            update.Close();

            //消息循环线程已退出,不会导致多线程问题
            if (callbackFunction.Get().OnDestroy)callbackFunction.Get().OnDestroy(*this);

            PostQuitMessage(0);
            break;
        }
        messageStream << Message(uMsg, wParam, lParam);
        if (UserProcessMessages)return UserProcessMessages(*this, uMsg, wParam, lParam);
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
    

}