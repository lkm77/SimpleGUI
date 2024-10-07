#include "Win32Window.h"

namespace SimpleGUI
{
    void Win32Window::CreateWin32Window()
    {
        Rect windowRect = AdjustWindow(wc);
        hwnd = CreateWindowEx
        (
            wc.style.exStyle,
            wc.wc.lpszClassName,
            wc.name.c_str(),
            wc.style.style,
            windowRect.left, windowRect.top,
            windowRect.Width(), windowRect.Height(),
            wc.parent, wc.menu,
            hinstance, NULL
        );
        if (!hwnd)
        {
            hwnd = nullptr;
            isCreating.store(false);
            return;
        }

        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)this);

        ShowWindow(hwnd, SW_SHOW);//SW_SHOW:原样显示
        UpdateWindow(hwnd);//刷新窗口

        isCreating.store (false);
        SendMessage(hwnd, WM_CREATE, 0, 0);
        MessageLoop();
    }

    void Win32Window::UpdateLoop()
    {
        taskCount++;//更新循环开始,计数器+1
        while (!closeWindow)
        {
            if(exchangeWindowManager)windowManager.swap(temporaryWindowManager);
            if(windowManager)windowManager->Update(*this);
            if (controlFrameRate)frameRateController.waitForNextFrame();
        }
        taskCount--;//更新循环结束,计数器-1
    }
    void Win32Window::MessageLoop()
    {
        taskCount++;//消息循环开始,计数器+1
        while (true)
        {
            if (closeWindow)DestroyWindow(hwnd);
            MSG msg;
            if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
            {
                if (msg.message == WM_QUIT)
                {
                    hwnd = nullptr;
                    taskCount--;//消息循环结束,计数器-1
                    closeWindow.store (false);
                    return;
                }
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
            Sleep(10);//减少CPU占用
            //if (controlFrameRate)frameRateController.waitForNextFrame();
        }
    }

    LRESULT Win32Window::ProcessMessages(UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        switch (uMsg)
        {
        case WM_CREATE:
        {
            if (windowManager)windowManager->Start(*this);
            //窗口创建完成后,启动消息循环线程
            std::thread(&Win32Window::UpdateLoop, this).swap(updateLoop);
            break;
        }
        case WM_SIZE:
        {
            wc.clientRect.SetSize(LOWORD(lParam), HIWORD(lParam));
            if (windowManager)windowManager->OnResize(*this);
            break;
        }
        case WM_PAINT:
        {
            /*
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
            SimpleGUI::String text = std::to_wstring(GetClientWidth()) + L" x " + std::to_wstring(GetClientHeight());
            DrawText(hdc, text.c_str(), -1, &ps.rcPaint, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
            EndPaint(hwnd, &ps);
            */
            break;
        }
        case WM_DESTROY:
            if (updateLoop.joinable()) updateLoop.join();//等待更新循环线程结束
            if (windowManager)windowManager->OnDestroy(*this);
            PostQuitMessage(0);
            break;
        }
        if (UserProcessMessages)return UserProcessMessages(*this, uMsg, wParam, lParam);
        switch (uMsg)
        {
        case WM_SIZE:
        case WM_PAINT:
        case WM_DESTROY:
            return 0;
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
    }

}