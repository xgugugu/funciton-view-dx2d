
#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <windowsx.h>
#include "graphics.h"
#include <cmath>
#include <string>
#include <functional>

class Window {
	private:
		static int y_dpi;
		static float dpi;
		static void paint(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
			PAINTSTRUCT ps;
			BeginPaint(hwnd, &ps);
			EndPaint(hwnd, &ps);
			RECT rc;
			GetClientRect(hwnd, &rc);
			D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
			        D2D1_RENDER_TARGET_TYPE_DEFAULT, D2D1::PixelFormat(
			            DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE),
			        0, 0, D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE, D2D1_FEATURE_LEVEL_DEFAULT);
			ID2D1HwndRenderTarget* pRT;
			getD2DFactory()->CreateHwndRenderTarget(
			    props, D2D1::HwndRenderTargetProperties(
			        hwnd, D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top)), &pRT);
			Graphics* graphics = new Graphics(pRT);
			paintMsg(*graphics);
			delete graphics;
			pRT->Release();
		}
		static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
			switch (uMsg) {
				case WM_DESTROY: //¹Ø±Õ
					PostQuitMessage(0);
					return 0;
				case WM_MOUSEMOVE:
					mouseMsg(WM_MOUSEMOVE, GET_X_LPARAM(lParam) / dpi, GET_Y_LPARAM(lParam) / dpi);
					return 0;
				case WM_LBUTTONDOWN: //×ó¼ü°´ÏÂ
					mouseMsg(WM_LBUTTONDOWN, GET_X_LPARAM(lParam) / dpi, GET_Y_LPARAM(lParam) / dpi);
					return 0;
				case WM_LBUTTONUP: //×ó¼üÌ§Æð
					mouseMsg(WM_LBUTTONUP, GET_X_LPARAM(lParam) / dpi, GET_Y_LPARAM(lParam) / dpi);
					return 0;
				case WM_LBUTTONDBLCLK: //×ó¼üË«»÷
					mouseMsg(WM_LBUTTONDBLCLK, GET_X_LPARAM(lParam) / dpi, GET_Y_LPARAM(lParam) / dpi);
					return 0;
				case WM_RBUTTONDOWN: //ÓÒ¼ü°´ÏÂ
					mouseMsg(WM_RBUTTONDOWN, GET_X_LPARAM(lParam) / dpi, GET_Y_LPARAM(lParam) / dpi);
					return 0;
				case WM_RBUTTONUP: //ÓÒ¼üÌ§Æð
					mouseMsg(WM_RBUTTONUP, GET_X_LPARAM(lParam) / dpi, GET_Y_LPARAM(lParam) / dpi);
					return 0;
				case WM_RBUTTONDBLCLK: //ÓÒ¼üË«»÷
					mouseMsg(WM_RBUTTONDBLCLK, GET_X_LPARAM(lParam) / dpi, GET_Y_LPARAM(lParam) / dpi);
					return 0;
				case WM_KEYDOWN: //¼üÅÌ°´ÏÂ
					keyMsg(WM_KEYDOWN, wParam);
					return 0;
				case WM_KEYUP: //¼üÅÌÌ§Æð
					keyMsg(WM_KEYDOWN, wParam);
					return 0;
				case WM_ERASEBKGND:
					return 0;
				case WM_PAINT: //ÖØ»æ
					paint(hwnd, uMsg, wParam, lParam);
					return 0;
				case WM_CREATE:
					createFinished(hwnd);
					return 0;
				case WM_SIZE:
					sendPaintMsg();
				default:
					if (defaultMsg(hwnd, uMsg, wParam, lParam)) return 0;
			}
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
		}
		static HWND hwnd;
	public:
		enum {
			MSG_MOUSEMOVE = WM_MOUSEMOVE,
			MSG_LBUTTONDOWN = WM_LBUTTONDOWN,
			MSG_LBUTTONUP = WM_LBUTTONUP,
			MSG_LBUTTONDBLCLK = WM_LBUTTONDBLCLK,
			MSG_RBUTTONDOWN = WM_RBUTTONDOWN,
			MSG_RBUTTONUP = WM_RBUTTONUP,
			MSG_RBUTTONDBLCLK = WM_RBUTTONDBLCLK,
			MSG_KEYDOWN = WM_KEYDOWN,
			MSG_KEYUP = WM_KEYUP
		};
		static void createFinished(HWND hwnd);
		static void mouseMsg(unsigned int type, float x, float y);
		static void keyMsg(unsigned int type, int key);
		static void paintMsg(Graphics &graphics);
		static bool defaultMsg(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		int create(unsigned int width, unsigned int height, const std::string &title) {
			HMODULE hModule = LoadLibrary("user32.dll");
			typedef BOOL(*WINAPI F_SPDPIA)();
			F_SPDPIA func = (F_SPDPIA)GetProcAddress(hModule, "SetProcessDPIAware");
			func();
			WNDCLASS wc = {};
			wc.lpfnWndProc = WindowProc;
			wc.hInstance = GetModuleHandle(NULL);
			wc.lpszClassName = "G-Window";
			wc.hbrBackground = CreateSolidBrush(RGB(0, 0, 0));
			wc.style = CS_DBLCLKS;
			RegisterClass(&wc);
			HDC hdc = GetDC(0);
			y_dpi = GetDeviceCaps(hdc, LOGPIXELSX);
			dpi = GetDeviceCaps(hdc, LOGPIXELSX) / 96.0f;
			DeleteObject(hdc);
			RECT rc = {};
			rc.right = roundf(width * dpi);
			rc.bottom = roundf(height * dpi);
			AdjustWindowRect(&rc, WS_VISIBLE | WS_OVERLAPPEDWINDOW, FALSE);
			hwnd = CreateWindowEx(0, "G-Window", title.c_str(), WS_VISIBLE | WS_OVERLAPPEDWINDOW,
			                      CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top,
			                      NULL, NULL, GetModuleHandle(NULL), NULL);
			MSG msg;
			while (GetMessage(&msg, NULL, 0, 0) > 0) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			return msg.wParam;
		}
		static void sendPaintMsg() {
			InvalidateRect(hwnd, NULL, 1);
			UpdateWindow(hwnd);
		}
		static void drawGraphics(std::function<void(Graphics &)> callback) {
			PAINTSTRUCT ps;
			BeginPaint(hwnd, &ps);
			EndPaint(hwnd, &ps);
			RECT rc;
			GetClientRect(hwnd, &rc);
			D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
			        D2D1_RENDER_TARGET_TYPE_DEFAULT, D2D1::PixelFormat(
			            DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE),
			        0, 0, D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE, D2D1_FEATURE_LEVEL_DEFAULT);
			ID2D1HwndRenderTarget* pRT;
			getD2DFactory()->CreateHwndRenderTarget(
			    props, D2D1::HwndRenderTargetProperties(
			        hwnd, D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top)), &pRT);
			Graphics* graphics = new Graphics(pRT);
			callback(*graphics);
			delete graphics;
			pRT->Release();
		}
		static double width() {
			RECT r;
			GetClientRect(hwnd, &r);
			return r.right * 1.0 / dpi;
		}
		static double height() {
			RECT r;
			GetClientRect(hwnd, &r);
			return r.bottom * 1.0 / dpi;
		}
};
int Window::y_dpi;
float Window::dpi;
HWND Window::hwnd;
