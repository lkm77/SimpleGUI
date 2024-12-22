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
		~Bitmap()
		{
			DeleteMemoryDC();
		}

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
			return memoryDC;
		}
		void Clear(const COLORREF color=RGB(240, 240, 240))
		{
			if (IsEmpty())return;
			RECT rect = { 0, 0, size.width, size.height };
			FillRect(memoryDC, &rect, (HBRUSH)(COLOR_WINDOW + 1));
		}

		void DrawToHdc(HDC hdc)
		{
			if (IsEmpty())return;
			BitBlt(hdc, 0, 0, size.width, size.height, memoryDC, 0, 0, SRCCOPY);
		}
		void DrawToHwnd(HWND hwnd)
		{
			if (IsEmpty())return;
			HDC hdc = GetDC(hwnd);
			DrawToHdc(hdc);
			ReleaseDC(hwnd, hdc);
		}

	private:
		void CreateMemoryDC()
		{
			DeleteMemoryDC();
			memoryDC = CreateCompatibleDC(NULL);
		}
		void DeleteMemoryDC()
		{
			if (memoryDC==nullptr)return;
			DeleteObject(SelectObject(memoryDC, memoryOldHBitmap));
			memoryOldHBitmap=nullptr;
			DeleteDC(memoryDC);
			memoryDC = nullptr;
		}
		void CreateBitmap(const int realWidth, const int realHeight)
		{
			if(memoryDC==nullptr)CreateMemoryDC();
			if(memoryDC==nullptr)return;
			HBITMAP hBitmap = CreateCompatibleBitmap(memoryDC, realWidth, realHeight);
			HBITMAP fromHBitmap = (HBITMAP)SelectObject(memoryDC, hBitmap);

			HDC fromHdc = CreateCompatibleDC(memoryDC);
			HBITMAP fromOldHBitmap = (HBITMAP)SelectObject(fromHdc, fromHBitmap);

			BitBlt(memoryDC, 0, 0, min(realWidth, realSize.width), min(realHeight, realSize.height), fromHdc, 0, 0, SRCCOPY);

			SelectObject(fromHdc, fromOldHBitmap);
			DeleteObject(fromHBitmap);
			DeleteDC(fromHdc);
			realSize.SetSize(realWidth, realHeight);
		}
		bool IsEmpty() const
		{
			return memoryDC==nullptr;
		}

		HDC memoryDC = nullptr;
		HBITMAP memoryOldHBitmap = nullptr;
	private:
		BitmapSize size;
		BitmapSize realSize;
	};
}
