#pragma once
#include "WindowProperties.h"
#include "CallbackFunction.h"
#include "Loop.h"
#include "ThreadLoop.h"
#include "Variable.h"
#include "Bitmap.h"
#include "IOStream.h"


namespace SimpleGUI
{
    class Win32Window;

    class WindowCallbackFunction
    {
    public:
        WindowCallbackFunction() {}
        template<typename T>
        WindowCallbackFunction(T&& t)
        {
            SetCallbackFromClass(std::forward<T>(t));
        }
        template<typename T>
        void SetCallbackFromClass(const T& t)
        {
            SimpleGUI_CallbackFunction_Set(t, Start);
            SimpleGUI_CallbackFunction_Set(t, OnDestroy);
            SimpleGUI_CallbackFunction_Set(t, OnResize);
            SimpleGUI_CallbackFunction_Set(t, Update);
        }
        SimpleGUI_CallbackFunction(void, Start,     Win32Window&);
        SimpleGUI_CallbackFunction(void, OnDestroy, Win32Window&);
        //注意:在OnResize与Update都有绘制操作时,会导致窗口闪烁
        SimpleGUI_CallbackFunction(void, OnResize, Win32Window&);
        SimpleGUI_CallbackFunction(void, Update,    Win32Window&);
    };

    struct Win32WindowMessage
    {
        Win32WindowMessage(UINT uMsg=NULL, WPARAM wParam=NULL, LPARAM lParam=NULL) :uMsg(uMsg), wParam(wParam), lParam(lParam){}
        UINT uMsg;
        WPARAM wParam;
        LPARAM lParam;
    };
    typedef Win32WindowMessage Message;
    typedef InputStream<Message> MessageInput;
    typedef IOStream<Message> MessageIO;

    class Win32Window
    {
    public:
        Win32Window() : hwnd(nullptr)
        {
            wc.wndClass.cbClsExtra = 0;//窗口类的附加数据buff的大小(buff:缓冲区)
            wc.wndClass.cbWndExtra = sizeof(Win32Window*);//窗口的附加数据buff的大小(buff:缓冲区)
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
        ~Win32Window()
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
        //获取窗口客户区矩形,客户区坐标系
        Rect GetClientRect() const { return Rect(0, 0, wc.clientRect.Width(), wc.clientRect.Height()); }
        //获取窗口客户区矩形,屏幕坐标系
        Rect GetClientRectScreen() const { return wc.clientRect; }
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

    private:
        //创建窗口
        void CreateWin32Window();
    private:
        HWND hwnd;  //窗口句柄
        Win32Class wc;  //窗口类
        bool isCreating=false;  //是否正在创建


    public:
        //设置窗口回调函数
        template<class T>
        void SetCallbackFunction(const T& t)
        {
            WindowCallbackFunction func(t);
            callbackFunction.Set(func);
        }
        //设置用户消息处理函数
        void SetUserProcessMessages(std::function<LRESULT(Win32Window& window, UINT uMsg, WPARAM wParam, LPARAM lParam)> func)
        {
            UserProcessMessages.Set(func);
        }
        void EnableUpdateFrameRateControl(bool enable) { updateFrameRateController.EnableFrameRateControl(enable); }
        void SetUpdateTargetFPS(int frameRate) { updateFrameRateController.SetTargetFPS(frameRate); }
    private:
        static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
        {
            Win32Window* pThis = nullptr;
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
        LRESULT WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam);//处理窗口消息
        void ProcessMessages();//消息循环

        void Update();
    private:
        Loop messageLoop;    //消息循环
        ThreadLoop update;   //更新线程循环
        FrameRateController updateFrameRateController;//更新帧率控制器
        std::atomic<bool> resizeFlag = false;

        Variable<WindowCallbackFunction> callbackFunction;//窗口回调函数
        Variable<std::function<LRESULT(Win32Window& window, UINT uMsg, WPARAM wParam, LPARAM lParam)> > UserProcessMessages;//用户消息处理函数
    public:
        HDC GetMemoryDC() const { return bitmap.GetMemoryDC(); }
    private:
        Bitmap bitmap= Bitmap(wc.clientRect.Width(), wc.clientRect.Height());


    public:
        MessageInput GetMessageStream() { return MessageInput(messageStream);}
    private:
        MessageIO messageStream;  //窗口消息流
    };
}

