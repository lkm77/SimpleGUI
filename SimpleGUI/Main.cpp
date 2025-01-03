#include "Win32Window.h"
#include <iostream>
class Renderer
{
public:
    Renderer() {}
    void OnResize(SimpleGUI::Win32Window& window)
    {
        std::cout << "Resize" << std::endl;
    }
    void Update(SimpleGUI::Win32Window& window)
    {
        //std::cout << "Update" << std::endl;
        HDC hdc = window.GetMemoryDC();
        RECT rc = RectToRECT(window.GetClientRect());
        FillRect(hdc, &rc, (HBRUSH)(COLOR_WINDOW + 1));
        Rectangle(hdc, x, y, x + 100, y + 100);
        SimpleGUI::String text = std::to_wstring(window.GetClientWidth()) + L" x " + std::to_wstring(window.GetClientHeight());
        SetBkMode(hdc, TRANSPARENT);
        DrawText(hdc, text.c_str(), -1, &rc, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
        x++;
        y++;
        if (x > window.GetClientWidth())x = y = 0;
        if (y > window.GetClientHeight())x = y = 0;
        
    }
private:
    int x = 0, y = 0;
};


int main()
{
    SimpleGUI::Win32Window window;
    window.SetCallbackFunction(Renderer());
    window.Create();
    while (window.Survival())Sleep(100);
	return 0;
}