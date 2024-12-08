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

    // Win32窗口样式
        //部分窗口样式
        // WS_CAPTION           窗口有标题栏
        // WS_CHILD             窗口为子窗口
        // WS_HSCROLL           窗口有水平滚动条
        // WS_VSCROLL           窗口有垂直滚动条
        // WS_MAXIMIZE          窗口为最大化状态
        // WS_MAXIMIZEBOX       窗口有最大化按钮
        // WS_MINIMIZE          窗口为最小化状态
        // WS_MINIMIZEBOX       窗口有最小化按钮
        // WS_OVERLAPPED        窗口为重叠样式
        // WS_OVERLAPPEDWINDOW  窗口为重叠窗口样式
        // WS_SIZEBOX           窗口有大小调整边框
        // WS_SYSMENU           窗口的标题栏上有窗口菜单
        // 
        // 部分扩展窗口样式
        // WS_EX_ACCEPTFILES 窗口接受文件拖放
        // WS_EX_WINDOWEDGE 窗口为可拖动样式
    struct Win32Style
    {
        DWORD style = 0; // 窗口样式
        DWORD exStyle = 0; // 扩展窗体样式

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

    //将客户区Rect换为相应的窗口Rect
    Rect AdjustWindow(const Win32Class& wc);

    //是否存在窗口类
    bool IsWindowClassRegistered(const Char* className);
}
