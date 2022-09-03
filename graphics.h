
#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <iostream>
#include <windows.h>
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>
#include <string>
#include <cmath>

namespace {
	ID2D1Factory* pD2DFactory;
	IDWriteFactory* pDWriteFactory;
	std::wstring s2ws(const std::string &s) {
		std::string curLocale = setlocale(LC_ALL, NULL);
		setlocale(LC_ALL, "chs");
		size_t charNum = sizeof(char) * s.size() + 1;
		wchar_t* dest = new wchar_t[charNum];
		mbstowcs(dest, s.c_str(), charNum);
		std::wstring result = dest;
		delete[] dest;
		setlocale(LC_ALL, curLocale.c_str());
		return result;
	}
}
void initGraphics() {
	D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, &pD2DFactory);
	DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), (IUnknown**)&pDWriteFactory);
}
void cleanGraphics() {
	pD2DFactory->Release();
	pDWriteFactory->Release();
}
ID2D1Factory* getD2DFactory() {
	return pD2DFactory;
}
class Graphics {
	private:
		ID2D1RenderTarget* rt;
		ID2D1SolidColorBrush* br;
		int getDPI() {
			HDC h = GetDC(0);
			int ret = GetDeviceCaps(h, LOGPIXELSX);
			DeleteDC(h);
			return ret;
		}
	public:
		template<typename T> Graphics(T* pRT) {
			rt = pRT;
			rt->BeginDraw();
			rt->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &br);
		}
		ID2D1RenderTarget* getRT() {
			return rt;
		}
		void clear(int r = 255, int g = 255, int b = 255) {
			rt->Clear(D2D1::ColorF(r / 255.0f, g / 255.0f, b / 255.0f));
		}
		ID2D1SolidColorBrush* color() {
			return br;
		}
		void color(int r, int g, int b) {
			br->SetColor(D2D1::ColorF(r / 255.0f, g / 255.0f, b / 255.0f));
		}
		void color(int r, int g, int b, int a) {
			br->SetColor(D2D1::ColorF(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f));
		}
		void drawLine(float x1, float y1, float x2, float y2, float width = 1.0f) {
			D2D1_POINT_2F p1, p2;
			p1.x = x1, p1.y = y1;
			p2.x = x2, p2.y = y2;
			rt->DrawLine(p1, p2, br, width, NULL);
		}
		void drawRect(float x, float y, float w, float h, float strokeWidth = 1.0f) {
			D2D1_RECT_F r;
			r.left = x, r.right = x + w;
			r.top = y, r.bottom = y + h;
			rt->DrawRectangle(r, br, strokeWidth, NULL);
		}
		void fillRect(float x, float y, float w, float h) {
			D2D1_RECT_F r;
			r.left = x, r.right = x + w;
			r.top = y, r.bottom = y + h;
			rt->FillRectangle(r, br);
		}
		void drawRoundedRect(float x, float y, float w, float h, float strokeWidth = 1.0f, float radius = 10.0f) {
			D2D1_ROUNDED_RECT rr = D2D1::RoundedRect(D2D1::RectF(x, y, x + w, y + h), radius, radius);
			rt->DrawRoundedRectangle(rr, br, strokeWidth);
		}
		void fillRoundedRect(float x, float y, float w, float h, float radius = 10.0f) {
			D2D1_ROUNDED_RECT rr = D2D1::RoundedRect(D2D1::RectF(x, y, x + w, y + h), radius, radius);
			rt->FillRoundedRectangle(rr, br);
		}
		void drawEllipse(float x, float y, float radiusX, float radiusY, float strokeWidth = 1.0f) {
			D2D1_ELLIPSE e;
			e.point.x = x, e.point.y = y, e.radiusX = radiusX, e.radiusY = radiusY;
			rt->DrawEllipse(e, br, strokeWidth, NULL);
		}
		void fillEllipse(float x, float y, float radiusX, float radiusY) {
			D2D1_ELLIPSE e;
			e.point.x = x, e.point.y = y, e.radiusX = radiusX, e.radiusY = radiusY;
			rt->FillEllipse(e, br);
		}
		//fontStyle(0-2)：0为正常，1为倾斜，2为斜体（默认为0）
		//fontWeight(1-999)：数值越低，表示字体越细；数值越高，表示字体越粗（默认为400正常）
		void drawText(std::string text, float x, float y, float w, float h, int align, float fontSize,
		              std::string fontName = "微软雅黑", unsigned int fontStyle = 0, unsigned int fontWeight = 400) {
			IDWriteTextFormat* tf;
			pDWriteFactory->CreateTextFormat(
			    s2ws(fontName).c_str(), NULL,
			    (DWRITE_FONT_WEIGHT)fontWeight, (DWRITE_FONT_STYLE)fontStyle, DWRITE_FONT_STRETCH_NORMAL,
			    fontSize, L"zh-cn", &tf);
			tf->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
			tf->SetTextAlignment((DWRITE_TEXT_ALIGNMENT)align);
			D2D1_RECT_F rc;
			rc.left = x, rc.right = x + w;
			rc.top = y, rc.bottom = y + h;
			rt->DrawText(s2ws(text).c_str(), s2ws(text).size(), tf, rc, br);
			tf->Release();
		}
		ID2D1RenderTarget* operator->() {
			return rt;
		}
		void release() {
			rt->EndDraw();
			rt->BeginDraw();
		}
		~Graphics() {
			rt->EndDraw();
			br->Release();
		}
};
