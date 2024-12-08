#pragma once
#include <atomic>
#include <functional>
#include <iostream>
#include <windows.h>

#include "Rect.h"
#include "String.h"
#include "Loop.h"
#include "ThreadLoop.h"
#include "Variable.h"

namespace SimpleGUI
{
    static HINSTANCE hinstance = GetModuleHandle(NULL);

    RECT RectToRECT(const Rect& rect);
    Rect RECTToRect(const RECT& rect);

    // Win32������ʽ
        //���ִ�����ʽ
        // WS_CAPTION           �����б�����
        // WS_CHILD             ����Ϊ�Ӵ���
        // WS_HSCROLL           ������ˮƽ������
        // WS_VSCROLL           �����д�ֱ������
        // WS_MAXIMIZE          ����Ϊ���״̬
        // WS_MAXIMIZEBOX       ��������󻯰�ť
        // WS_MINIMIZE          ����Ϊ��С��״̬
        // WS_MINIMIZEBOX       ��������С����ť
        // WS_OVERLAPPED        ����Ϊ�ص���ʽ
        // WS_OVERLAPPEDWINDOW  ����Ϊ�ص�������ʽ
        // WS_SIZEBOX           �����д�С�����߿�
        // WS_SYSMENU           ���ڵı��������д��ڲ˵�
        // 
        // ������չ������ʽ
        // WS_EX_ACCEPTFILES ���ڽ����ļ��Ϸ�
        // WS_EX_WINDOWEDGE ����Ϊ���϶���ʽ
    struct Win32Style
    {
        DWORD style = 0; // ������ʽ
        DWORD exStyle = 0; // ��չ������ʽ

        Win32Style() {}
        Win32Style(DWORD style, DWORD exStyle)
        {
            this->style = style;
            this->exStyle = exStyle;
        }

        void AddStyle(DWORD style) { this->style |= style; }
        void DeleteStyle(DWORD style) { this->style &= (~style); }
        void AddExStyle(DWORD exStyle) { this->exStyle |= exStyle; }
        void DeleteExStyle(DWORD exStyle) { this->exStyle &= (~exStyle); }
    };

    struct Win32Class
    {
        WNDCLASS wndClass = {};
        HWND parent = nullptr;
        HMENU menu = nullptr;

        Rect clientRect;
        SimpleGUI::String name = TEXT("Win32Window");
        SimpleGUI::Win32Style style = { WS_OVERLAPPEDWINDOW,NULL };


        Win32Class()
        {
            clientRect.SetPosition(100, 100);
            clientRect.SetSize(1080, 960);
        }

        bool HaveParent()const { return parent != nullptr; }
        bool HaveMenu()const { return menu != nullptr; }
    };

    //���ͻ���Rect��Ϊ��Ӧ�Ĵ���Rect
    Rect AdjustWindow(const Win32Class& wc);

    //�Ƿ���ڴ�����
    bool IsWindowClassRegistered(const Char* className);
}
