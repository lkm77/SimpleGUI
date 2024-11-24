#include "Win32Window.h"
#include <iostream>

class MyManager : public SimpleGUI::WindowManager
{
public:
    MyManager() {}
    void OnStart(SimpleGUI::Win32Window& window) override
    {
        std::cout << "OnStart" << std::endl;
        HWND hwnd = window.GetHwnd();
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
        SimpleGUI::String text = std::to_wstring(window.GetClientWidth()) + L" x " + std::to_wstring(window.GetClientHeight());
        DrawText(hdc, text.c_str(), -1, &ps.rcPaint, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
        EndPaint(hwnd, &ps);
    }
    void OnResize(SimpleGUI::Win32Window& window) override
    {
        std::cout << "Resize" << std::endl;
        HWND hwnd = window.GetHwnd();
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
        SimpleGUI::String text = std::to_wstring(window.GetClientWidth()) + L" x " + std::to_wstring(window.GetClientHeight());
        DrawText(hdc, text.c_str(), -1, &ps.rcPaint, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
        EndPaint(hwnd, &ps);
    }
    void OnUpdate(SimpleGUI::Win32Window& window) override
    {
        std::cout << "Update" << std::endl;
    }
};

int main()
{
    SimpleGUI::Win32Window window;
    window.SetWindowManager(std::make_unique<MyManager>());
    window.Create();
    while (window.Survival())Sleep(100);
	return 0;
}
