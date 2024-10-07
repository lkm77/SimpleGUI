#pragma once
#include "WindowProperties.h"
#include <cassert>

namespace SimpleGUI
{
    class Win32Window;//forward declaration

    //名称暂定，想到更好的名字再改
    class WindowManager
    {
    public:
        WindowManager() {}
        virtual ~WindowManager() {}
    public:
        //窗口创建时调用
        virtual void Start(Win32Window&) {}
        //窗口销毁时调用
        virtual void OnDestroy(Win32Window&) {}
        //窗口大小改变时调用
        virtual void OnResize(Win32Window&) {}
        //每帧调用
        virtual void Update(Win32Window&) {}
    };

    class Win32Window
    {
    public:

        static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
        {
            Win32Window* pThis = (Win32Window*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

            if (pThis) return pThis->ProcessMessages(uMsg, wParam, lParam);
            else return DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
        Win32Window() : hwnd(nullptr)
        {
            //窗口类的附加数据buff的大小(buff:缓冲区)
            wc.wc.cbClsExtra = 0;
            //窗口的附加数据buff的大小(buff:缓冲区)
            wc.wc.cbWndExtra = 0;
            //绘制窗口背景的画刷句柄(窗口背景颜色)
            wc.wc.hbrBackground = NULL;
            //鼠标的句柄[窗口光标(NULL:默认光标)]
            wc.wc.hCursor = NULL;
            //窗口图标(NULL:默认图标)
            wc.wc.hIcon = NULL;
            //当前模块的实例句柄
            wc.wc.hInstance = SimpleGUI::hinstance;
            //窗口处理函数
            wc.wc.lpfnWndProc = WindowProc;
            //窗口类的名称
            wc.wc.lpszClassName = TEXT("Win32WindowClass");
            //窗口菜单的资源ID字符串[窗口菜单(NULL:无菜单)]
            wc.wc.lpszMenuName = NULL;
            //style:窗口类风格
            wc.wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
            //RegisterClass会检查是否已经注册过，如果已经注册过，则不再注册
            RegisterClass(&wc.wc);
        }
        virtual ~Win32Window()
        {
            Close();
            if (messageLoop.joinable()) messageLoop.join();
            //UnregisterClass会检查是否已经注册过，如果没有注册过，则不再注销
            UnregisterClass(wc.wc.lpszClassName, hinstance);
        }

        //创建窗口
        void Create()
        {
            isCreating.store(true);
            std::thread(&Win32Window::CreateWin32Window, this).swap(messageLoop);
            while (isCreating)Sleep(100);
        }
        //创建窗口
        void Create(const Win32Class& wc)
        {
            this->wc = wc;
            isCreating.store(true);
            std::thread(&Win32Window::CreateWin32Window, this).swap(messageLoop);
            while (isCreating)Sleep(100);
        }
        bool IsRunning() const 
        { 
            assert(taskCount >= 0);//断言:任务计数不能为负
            return taskCount > 0;
        }
        void Close(bool wait=true)
        { 
            closeWindow.store(true);
            if (!wait)return;//不等待的话直接返回
            if(messageLoop.joinable()) messageLoop.join();
        }


        HWND GetHwnd() const { return hwnd; }
        int GetClientWidth() const { return wc.clientRect.Width(); }
        int GetClientHeight() const { return wc.clientRect.Height(); }
        const Win32Class& GetWin32Class() const { return wc; }

        void SetPosition(int x, int y)
        {
            Rect re = wc.clientRect;
            re.SetPosition(x, y);
            SetClientRect(re);
        }
        void SetClientSize(int width, int height)
        {
            Rect re = wc.clientRect;
            re.SetSize(width, height);
            SetClientRect(re);
        }
        void SetClientRect(const Rect& clientRect)
        {
            if (!hwnd)return;
            wc.clientRect = clientRect;
            Rect rect = AdjustWindow(wc);
            MoveWindow(hwnd, rect.left, rect.top, rect.Width(), rect.Height(), TRUE);
        }

        void ControlFrameRate(bool enable) { controlFrameRate.store (enable); }
        void SetFrameRate(int frameRate) { frameRateController.setTargetFPS(frameRate); }

        void SetUserProcessMessages(std::function<LRESULT(Win32Window& window, UINT uMsg, WPARAM wParam, LPARAM lParam)> func)
            { UserProcessMessages = func; }

        //设置窗口管理器
        std::unique_ptr<WindowManager> SetWindowManager(std::unique_ptr<WindowManager> windowManager)
        {
            this->temporaryWindowManager.reset(windowManager.release());
            exchangeWindowManager.store(true);
            while (exchangeWindowManager) Sleep(100);
            return std::move(temporaryWindowManager);
        }
        //获取窗口管理器所有权
        std::unique_ptr<WindowManager> ReleaseWindowManager()
        {
            exchangeWindowManager.store(true);
            while (exchangeWindowManager) Sleep(100);
            return std::move(temporaryWindowManager);
        }

    private:
        //创建窗口
        void CreateWin32Window();
        void UpdateLoop();
        //消息循环
        void MessageLoop();
        //处理窗口消息
        LRESULT ProcessMessages(UINT uMsg, WPARAM wParam, LPARAM lParam);
    private:
        HWND hwnd;  //窗口句柄
        Win32Class wc;  //窗口类
        std::function<LRESULT(Win32Window& window, UINT uMsg, WPARAM wParam, LPARAM lParam)> UserProcessMessages;

        std::atomic<bool> isCreating=false;  //是否正在创建
        std::atomic<bool> closeWindow=false;  //是否需要关闭窗口
        std::atomic<int> taskCount=0;  //任务计数,用于判断窗口是否正在运行
        std::thread messageLoop;  //消息循环线程
        std::thread updateLoop;  //更新循环线程

        //是否需要控制windowManager的Update函数的调用频率
        std::atomic<bool> controlFrameRate=true;
        //控制windowManager的Update函数的调用频率
        FrameRateController frameRateController;


        //交换窗口管理器标志
        std::atomic<bool> exchangeWindowManager=false;
        std::unique_ptr<WindowManager> windowManager = nullptr;
        std::unique_ptr<WindowManager> temporaryWindowManager=nullptr;
    };
}

