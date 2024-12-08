#pragma once
#include <windows.h>
namespace SimpleGUI
{
	class Win32Bitmap
	{
	public:
		Win32Bitmap()
		{
			hdc = GetDC(nullptr);
			memdc = CreateCompatibleDC(hdc);
			bitmap = CreateCompatibleBitmap(memdc, width, height);
		}
		~Win32Bitmap()
		{
			DeleteObject(bitmap);
			DeleteDC(memdc);
			ReleaseDC(nullptr, hdc);
		}
	public:
		void SetHdc(const HDC hdc)
		{
			this->hdc = hdc;
			DeleteObject(bitmap);
			DeleteDC(memdc);
			memdc = CreateCompatibleDC(hdc);
			bitmap = CreateCompatibleBitmap(memdc, width, height);
		}
		//设置位图大小,使其至少为指定大小
		void Resize(const int w, const int h)
		{
			width = w;
			height = h;
			if (w <= 0 || h <= 0)return;
			if (w <= width && h <= height)return;
			realWidth = w;
			realHeight = h;
			DeleteObject(bitmap);
			bitmap = CreateCompatibleBitmap(memdc, width, height);
		}
		//获取大小
		int GetWidth() const { return width; }
		int GetHeight() const { return height; }
		//获取实际大小
		int GetRealWidth() const { return realWidth; }
		int GetRealHeight() const { return realHeight; }
		//获取位图句柄
		HBITMAP GetBitmap() const { return bitmap; }
		//获取内存设备上下文
		HDC GetMemDc() const { return memdc; }
		void Clear(const COLORREF color) const
		{
			HGDIOBJ oldBitmap = SelectObject(memdc, bitmap);
			SetBkColor(memdc, color);
			Rectangle(memdc, 0, 0, width, height);
			SelectObject(memdc, oldBitmap);
		}
		void CopyToHdc(const HDC hdcDest, const int x = 0, const int y = 0) const
		{
			BitBlt(hdcDest, x, y, width, height, memdc, 0, 0, SRCCOPY);
		}
	private:
		HDC hdc = nullptr;
		HDC memdc = nullptr;
		int width=1080, height=960;
		int realWidth=1080, realHeight=960;
		HBITMAP bitmap = nullptr;
	};
}

