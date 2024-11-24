#pragma once
#include "WindowProperties.h"
#include <mutex>

namespace SimpleGUI
{
    class Win32Window;

    //�����ݶ����뵽���õ������ٸ�
    class WindowManager
    {
    public:
        WindowManager() {}
        virtual ~WindowManager() {}
    public:
        //���ڴ���ʱ����
        virtual void OnStart(Win32Window&) {}
        //��������ʱ����
        virtual void OnDestroy(Win32Window&) {}
        //���ڴ�С�ı�ʱ����
        virtual void OnResize(Win32Window&) {}
        //ÿ֡����
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
            wc.wndClass.cbClsExtra = 0;//������ĸ�������buff�Ĵ�С(buff:������)
            wc.wndClass.cbWndExtra = 0;//���ڵĸ�������buff�Ĵ�С(buff:������)
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
        virtual ~Win32Window()
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

        void ControlFrameRate(const bool enable) { update.EnableFrameRateControl(enable); }
        void SetFrameRate(const int frameRate) { update.SetFrameRate(frameRate); }

        //���ô��ڹ�����
        void SetWindowManager(std::unique_ptr<WindowManager> manager)
        {
            windowManager.Set(std::move(manager));
        }
        //�����û���Ϣ������
        void SetUserProcessMessages(std::function<LRESULT(Win32Window& window, UINT uMsg, WPARAM wParam, LPARAM lParam)> func)
        {
            UserProcessMessages.Set(func);
        }

    private:
        //��������
        void CreateWin32Window();
        void Update();
        //��Ϣѭ��
        void ProcessMessages();
        //��������Ϣ
        LRESULT WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
    private:
        HWND hwnd;  //���ھ��
        Win32Class wc;  //������

        bool isCreating=false;  //�Ƿ����ڴ���
        Loop messageLoop;    //��Ϣѭ��
        ThreadLoop update;   //�����߳�ѭ��

        Variable<std::unique_ptr<WindowManager> > windowManager;//���ڹ�����
        Variable<std::function<LRESULT(Win32Window& window, UINT uMsg, WPARAM wParam, LPARAM lParam)> > UserProcessMessages;//�û���Ϣ������
    };
}

