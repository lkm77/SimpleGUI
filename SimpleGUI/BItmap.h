#pragma once
#include <Windows.h>

namespace SimpleGUI
{

	struct BitmapSize
	{
		BitmapSize(const int w = 0, const int h = 0)
		{
			width = w;
			height = h;
		}
		void SetWidth(const int w)
		{
			width = w;
		}
		void SetHeight(const int h)
		{
			height = h;
		}
		void SetSize(const int w, const int h)
		{
			width = w;
			height = h;
		}
		int width;
		int height;
	};
	class Bitmap
	{
	public:
		Bitmap()
		{

		}
		Bitmap(const int w, const int h)
		{
			SetSize(w, h);
		}
		Bitmap(const BitmapSize size)
		{
			SetSize(size);
		}
		~Bitmap() {}

		void SetWidth(const int w)
		{
			SetSize(w, size.height);
		}
		void SetHeight(const int h)
		{
			SetSize(size.width, h);
		}
		void SetSize(const int w, const  int h)
		{
			if (w < 0 || h < 0)return;
			size.SetSize(w, h);
			if (w <= realSize.width && h <= realSize.height)return;
			CreateBitmap(w, h);
		}
		void SetSize(const BitmapSize size)
		{
			SetSize(size.width, size.height);
		}
		void SetRealWidthAndWidth(const int w)
		{
			SetSize(w, realSize.height);
		}
		void SetRealHeightAndHeight(const int h)
		{
			SetSize(realSize.width, h);
		}
		void SetRealSizeAndSize(const int w, const int h)
		{
			if (w < 0 || h < 0)return;
			size.SetSize(w, h);
			CreateBitmap(w, h);
		}
		void SetRealSize(const BitmapSize size)
		{
			SetRealSizeAndSize(size.width, size.height);
		}

		BitmapSize GetSize() const
		{
			return size;
		}
		BitmapSize GetRealSize() const
		{
			return realSize;
		}

		HDC GetMemoryDC() const
		{
			return hdcMem;
		}
		void Clear(const COLORREF color=RGB(240, 240, 240))
		{
			if (IsEmpty())return;
			RECT rect = { 0, 0, size.width, size.height };
			FillRect(hdcMem, &rect, (HBRUSH)(COLOR_WINDOW + 1));
		}

		void DrawToHdc(HDC hdc)
		{
			if (IsEmpty())return;
			BitBlt(hdc, 0, 0, size.width, size.height, hdcMem, 0, 0, SRCCOPY);
		}
		void DrawToHwnd(HWND hwnd)
		{
			if (IsEmpty())return;
			HDC hdc = GetDC(hwnd);
			DrawToHdc(hdc);
			ReleaseDC(hwnd, hdc);
		}

	private:
		void CreateHdcMem()
		{
			DeleteHdcMem();
			HDC hdc = GetDC(NULL);// 创建一个与设备相关的内存DC
			hdcMem = CreateCompatibleDC(hdc);
			ReleaseDC(NULL, hdc);
		}
		void DeleteHdcMem()
		{
			if (hdcMem == nullptr)return;
			DeleteBitmap();
			DeleteDC(hdcMem);
			hdcMem = nullptr;
		}
		bool CreateBitmap(const int realWidht, const int realHeight)
		{
			if (realWidht < 0 || realHeight < 0)return false;
			DeleteBitmap();
			realSize.SetSize(realWidht, realHeight);
			if (hdcMem == nullptr)CreateHdcMem();
			if (hdcMem == nullptr)return false;
			HBITMAP hBitmap = CreateCompatibleBitmap(hdcMem, realWidht, realHeight);
			if (hBitmap == nullptr)return false;
			oldBitmap = SelectObject(hdcMem, hBitmap);
			return true;
		}
		void DeleteBitmap()
		{
			if (oldBitmap != nullptr)DeleteObject(SelectObject(hdcMem, oldBitmap));
		}
		bool IsEmpty() const
		{
			return !(oldBitmap && hdcMem);
		}
	private:
		HDC hdcMem = nullptr;
		HGDIOBJ oldBitmap = nullptr;
		BitmapSize size;
		BitmapSize realSize;
	};
}
