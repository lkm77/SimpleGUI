#pragma once
#include "WindowProperties.h"
#include <mutex>

namespace SimpleGUI
{
    class Win32Window;

    //名称暂定，想到更好的名字再改
    class WindowManager
    {
    public:
        WindowManager() {}
        virtual ~WindowManager() {}
    public:
        //窗口创建时调用
        virtual void OnStart(Win32Window&) {}
        //窗口销毁时调用
        virtual void OnDestroy(Win32Window&) {}
        //窗口大小改变时调用
        virtual void OnResize(Win32Window&) {}
        //每帧调用
        virtual void OnUpdate(Win32Window&) {}
    };

    class Win32Window
    {
    public:

        static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
        {
            Win32Window* pThis=nullptr;
            if (uMsg == WM_NCCREATE)
            {
                CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
                pThis = (Win32Window*)pCreate->lpCreateParams;
                pThis->hwnd = hwnd;
                SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pCreate->lpCreateParams);
            }
            else pThis = (Win32Window*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

            if (pThis) return pThis->WindowProc(uMsg, wParam, lParam);
            else return DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
        Win32Window() : hwnd(nullptr)
        {
            wc.wndClass.cbClsExtra = 0;//窗口类的附加数据buff的大小(buff:缓冲区)
            wc.wndClass.cbWndExtra = 0;//窗口的附加数据buff的大小(buff:缓冲区)
            wc.wndClass.hbrBackground = NULL;//绘制窗口背景的画刷句柄(窗口背景颜色)
            wc.wndClass.hCursor = NULL;//鼠标的句柄[窗口光标(NULL:默认光标)]
            wc.wndClass.hIcon = NULL; //窗口图标(NULL:默认图标)
            wc.wndClass.hInstance = SimpleGUI::hinstance;//当前模块的实例句柄
            wc.wndClass.lpfnWndProc = WindowProc;//窗口处理函数
            wc.wndClass.lpszClassName = TEXT("Win32WindowClass");//窗口类的名称
            wc.wndClass.lpszMenuName = NULL;//窗口菜单的资源ID字符串[窗口菜单(NULL:无菜单)]
            wc.wndClass.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;//style:窗口类风格
            
            RegisterClass(&wc.wndClass);//RegisterClass会检查是否已经注册过，如果已经注册过，则不再注册
        }
        virtual ~Win32Window()
        {
            Close(true);
            UnregisterClass(wc.wndClass.lpszClassName, hinstance);//UnregisterClass会检查是否已经注册过，如果没有注册过，则不再注销
        }

        //创建窗口,若窗口已经存在,则直接返回
        void Create()
        {
            if(Survival())return;
            isCreating=true;
            std::thread(&Win32Window::CreateWin32Window, this).detach();
            while (isCreating)Sleep(100);
        }
        //创建窗口,若窗口已经存在,则直接返回
        void Create(const Win32Class& wc)
        {
            this->wc = wc;
            Create();
        }
        //窗口是否存存活
        bool Survival() const 
        { 
            return messageLoop.IsLooping();
        }
        //关闭窗口
        void Close(const bool wait=true)
        { 
            messageLoop.Exit(wait);
        }


        //获取hwnd
        HWND GetHwnd() const { return hwnd; }
        //获取窗口客户区宽度
        int GetClientWidth() const { return wc.clientRect.Width(); }
        //获取窗口客户区高度
        int GetClientHeight() const { return wc.clientRect.Height(); }
        //获取Win32Class
        const Win32Class& GetWin32Class() const { return wc; }

        //设置窗口客户区位置
        void SetClientPosition(const int x, const int y)
        {
            Rect re = wc.clientRect;
            re.SetPosition(x, y);
            SetClientRect(re);
        }
        //设置窗口客户区大小
        void SetClientSize(const int width, const int height)
        {
            Rect re = wc.clientRect;
            re.SetSize(width, height);
            SetClientRect(re);
        }
        //设置窗口客户区位置及大小
        void SetClientRect(const Rect& clientRect)
        {
            if (!hwnd)return;
            wc.clientRect = clientRect;
            Rect rect = AdjustWindow(wc);
            MoveWindow(hwnd, rect.left, rect.top, rect.Width(), rect.Height(), TRUE);
        }

        void ControlFrameRate(const bool enable) { update.EnableFrameRateControl(enable); }
        void SetFrameRate(const int frameRate) { update.SetFrameRate(frameRate); }

        //设置窗口管理器
        void SetWindowManager(std::unique_ptr<WindowManager> manager)
        {
            windowManager.Set(std::move(manager));
        }
        //设置用户消息处理函数
        void SetUserProcessMessages(std::function<LRESULT(Win32Window& window, UINT uMsg, WPARAM wParam, LPARAM lParam)> func)
        {
            UserProcessMessages.Set(func);
        }

    private:
        //创建窗口
        void CreateWin32Window();
        void Update();
        //消息循环
        void ProcessMessages();
        //处理窗口消息
        LRESULT WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
    private:
        HWND hwnd;  //窗口句柄
        Win32Class wc;  //窗口类

        bool isCreating=false;  //是否正在创建
        Loop messageLoop;    //消息循环
        ThreadLoop update;   //更新线程循环

        Variable<std::unique_ptr<WindowManager> > windowManager;//窗口管理器
        Variable<std::function<LRESULT(Win32Window& window, UINT uMsg, WPARAM wParam, LPARAM lParam)> > UserProcessMessages;//用户消息处理函数
    };
}

