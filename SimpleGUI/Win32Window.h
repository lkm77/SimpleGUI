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
        //ע��:��OnResize��Update���л��Ʋ���ʱ,�ᵼ�´�����˸
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
            wc.wndClass.cbClsExtra = 0;//������ĸ�������buff�Ĵ�С(buff:������)
            wc.wndClass.cbWndExtra = sizeof(Win32Window*);//���ڵĸ�������buff�Ĵ�С(buff:������)
            wc.wndClass.hbrBackground = NULL;//���ƴ��ڱ����Ļ�ˢ���(���ڱ�����ɫ)
            wc.wndClass.hCursor = NULL;//���ľ��[���ڹ��(NULL:Ĭ�Ϲ��)]
            wc.wndClass.hIcon = NULL; //����ͼ��(NULL:Ĭ��ͼ��)
            wc.wndClass.hInstance = SimpleGUI::hinstance;//��ǰģ���ʵ�����
            wc.wndClass.lpfnWndProc = WindowProc;//���ڴ�����
            wc.wndClass.lpszClassName = TEXT("Win32WindowClass");//�����������
            wc.wndClass.lpszMenuName = NULL;//���ڲ˵�����ԴID�ַ���[���ڲ˵�(NULL:�޲˵�)]
            wc.wndClass.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;//style:��������
            
            RegisterClass(&wc.wndClass);//RegisterClass�����Ƿ��Ѿ�ע���������Ѿ�ע���������ע��
        }
        ~Win32Window()
        {
            Close(true);
            UnregisterClass(wc.wndClass.lpszClassName, hinstance);//UnregisterClass�����Ƿ��Ѿ�ע��������û��ע���������ע��
        }

        //��������,�������Ѿ�����,��ֱ�ӷ���
        void Create()
        {
            if(Survival())return;
            isCreating=true;
            std::thread(&Win32Window::CreateWin32Window, this).detach();
            while (isCreating)Sleep(100);
        }
        //��������,�������Ѿ�����,��ֱ�ӷ���
        void Create(const Win32Class& wc)
        {
            this->wc = wc;
            Create();
        }
        //�����Ƿ����
        bool Survival() const 
        { 
            return messageLoop.IsLooping();
        }
        //�رմ���
        void Close(const bool wait=true)
        { 
            messageLoop.Exit(wait);
        }


        //��ȡhwnd
        HWND GetHwnd() const { return hwnd; }
        //��ȡ���ڿͻ������
        int GetClientWidth() const { return wc.clientRect.Width(); }
        //��ȡ���ڿͻ����߶�
        int GetClientHeight() const { return wc.clientRect.Height(); }
        //��ȡ���ڿͻ�������,�ͻ�������ϵ
        Rect GetClientRect() const { return Rect(0, 0, wc.clientRect.Width(), wc.clientRect.Height()); }
        //��ȡ���ڿͻ�������,��Ļ����ϵ
        Rect GetClientRectScreen() const { return wc.clientRect; }
        //��ȡWin32Class
        const Win32Class& GetWin32Class() const { return wc; }


        //���ô��ڿͻ���λ��
        void SetClientPosition(const int x, const int y)
        {
            Rect re = wc.clientRect;
            re.SetPosition(x, y);
            SetClientRect(re);
        }
        //���ô��ڿͻ�����С
        void SetClientSize(const int width, const int height)
        {
            Rect re = wc.clientRect;
            re.SetSize(width, height);
            SetClientRect(re);
        }
        //���ô��ڿͻ���λ�ü���С
        void SetClientRect(const Rect& clientRect)
        {
            if (!hwnd)return;
            wc.clientRect = clientRect;
            Rect rect = AdjustWindow(wc);
            MoveWindow(hwnd, rect.left, rect.top, rect.Width(), rect.Height(), TRUE);
        }

    private:
        //��������
        void CreateWin32Window();
    private:
        HWND hwnd;  //���ھ��
        Win32Class wc;  //������
        bool isCreating=false;  //�Ƿ����ڴ���


    public:
        //���ô��ڻص�����
        template<class T>
        void SetCallbackFunction(const T& t)
        {
            WindowCallbackFunction func(t);
            callbackFunction.Set(func);
        }
        //�����û���Ϣ������
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
        LRESULT WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam);//��������Ϣ
        void ProcessMessages();//��Ϣѭ��

        void Update();
    private:
        Loop messageLoop;    //��Ϣѭ��
        ThreadLoop update;   //�����߳�ѭ��
        FrameRateController updateFrameRateController;//����֡�ʿ�����
        std::atomic<bool> resizeFlag = false;

        Variable<WindowCallbackFunction> callbackFunction;//���ڻص�����
        Variable<std::function<LRESULT(Win32Window& window, UINT uMsg, WPARAM wParam, LPARAM lParam)> > UserProcessMessages;//�û���Ϣ������
    public:
        HDC GetMemoryDC() const { return bitmap.GetMemoryDC(); }
    private:
        Bitmap bitmap= Bitmap(wc.clientRect.Width(), wc.clientRect.Height());


    public:
        MessageInput GetMessageStream() { return MessageInput(messageStream);}
    private:
        MessageIO messageStream;  //������Ϣ��
    };
}

