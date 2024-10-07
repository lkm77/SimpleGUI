#include "WindowProperties.h"
namespace SimpleGUI
{

    RECT RectToRECT(const Rect& rect)
    {
        RECT rc = { rect.left, rect.top, rect.right, rect.bottom };
        return rc;
    }
    Rect RECTToRect(const RECT& rect)
    {
        return Rect(rect.left, rect.top, rect.right, rect.bottom);
    }
    Rect AdjustWindow(const Win32Class& wc)
    {
        RECT rect = RectToRECT(wc.clientRect);
        AdjustWindowRectEx(
            &rect,
            wc.style.style,
            wc.HaveMenu(),
            wc.style.exStyle
        );
        return RECTToRect(rect);
    }

    bool IsWindowClassRegistered(const Char* className)
    {
        WNDCLASS existingWc;
        return GetClassInfo(SimpleGUI::hinstance, className, &existingWc);
    }

}