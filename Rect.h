#pragma once

namespace SimpleGUI
{
    struct Rect
    {
        int left = 0;
        int top = 0;
        int right = 0;
        int bottom = 0;
        Rect() {}
        Rect(int l, int t, int r, int b) : left(l), top(t), right(r), bottom(b) {}

        int Width() const
        {
            return this->right - this->left;
        }
        int Height() const
        {
            return this->bottom - this->top;
        }

        void ResetRect(int l, int t, int r, int b)
        {
            this->left = l;
            this->top = t;
            this->right = r;
            this->bottom = b;
        }
        void SetWidth(int w)
        {
            this->right = this->left + w;
        }
        void SetHeight(int h)
        {
            this->bottom = this->top + h;
        }
        void SetSize(int w, int h)
        {
            this->SetWidth(w);
            this->SetHeight(h);
        }
        //×óÉÏ½Ç×ø±ê
        void SetPosition(int x, int y)
        {
            this->right = x + this->Width();
            this->bottom = y + this->Height();
            this->left = x;
            this->top = y;
        };
    };
}