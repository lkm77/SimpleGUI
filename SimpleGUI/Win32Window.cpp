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
        ShowWindow(hwnd, SW_SHOW);//SW_SHOW:ԭ����ʾ
        isCreating=false;

        messageLoop.Start(&Win32Window::ProcessMessages, this);
    }

    void Win32Window::Update()
    {
        {
            //Ϊ�˱�����߳�����,Update��Resize������ͬһ�߳���ִ��
            //{}Ϊ������true_const��Χ
            bool true_const = true;//��ֹ�޸�,������compare_exchange_strong
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
        Sleep(10);//����CPUռ��
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
        Sleep(10);//����CPUռ��
    }

    LRESULT Win32Window::WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        switch (uMsg)
        {
        case WM_CREATE:
        {
            //��Ϣѭ���߳�δ����,���ᵼ�¶��߳�����
            if (callbackFunction.Get().Start)callbackFunction.Get().Start(*this);
            //���ڴ�����ɺ�,������Ϣѭ���߳�
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

            //��Ϣѭ���߳����˳�,���ᵼ�¶��߳�����
            if (callbackFunction.Get().OnDestroy)callbackFunction.Get().OnDestroy(*this);

            PostQuitMessage(0);
            break;
        }
        messageStream << Message(uMsg, wParam, lParam);
        if (UserProcessMessages)return UserProcessMessages(*this, uMsg, wParam, lParam);
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
    

}