
#include "window.h"
#include <map>
#include <conio.h>

using namespace std;

string tostr(double d) {
	char ret[256] = {};
	sprintf(ret, "%.1lf", d);
	return ret;
}

double x = 0, y = 0;
map<double, double> ans;

function<void(double)> make;

Window window;
int mouse_x = 300, mouse_y = 240;
void Window::mouseMsg(unsigned int type, float x, float y) {
	if (type == MSG_MOUSEMOVE) {
		mouse_x = x, mouse_y = y;
		sendPaintMsg();
	}
}
void Window::keyMsg(unsigned int type, int key) {
	if (type == MSG_KEYDOWN) {
		if (key == VK_RIGHT) {
			x += 10;
			sendPaintMsg();
		} else if (key == VK_LEFT) {
			x -= 10;
			sendPaintMsg();
		} else if (key == VK_UP) {
			y += 10;
			sendPaintMsg();
		} else if (key == VK_DOWN) {
			y -= 10;
			sendPaintMsg();
		}
	}
}
void Window::paintMsg(Graphics &g) {
	double w = window.width(), h = window.height();
	g.clear();
	g.color(0, 0, 0);
	g.drawLine(0, h / 2 + y, w, h / 2 + y);
	g.drawText("x", w - 15, h / 2 + y - 15, 15, 10, 2, 12, "微软雅黑", 1);
	g.drawLine(w / 2 - x, 0, w / 2 - x, h);
	g.drawText("y", w / 2 - x, 0, 15, 10, 2, 12, "微软雅黑", 1);
	g.drawText("0", w / 2 - x - 205, h / 2 + y, 200, 20, 1, 10);
	for (int i = x - w / 2; i <= x + w / 2; i++) {
		int sss = to_string(int(i / 10.0)).size() * 10;
		if (i % (int(i / 10.0) >= 0 ? sss : sss - 10) == 0 && (i < -100 || i > 100 || i % 20 == 0) && i != 0) {
			g.drawText(to_string(int(i / 10.0)), i + w / 2 - x - 100, h / 2 + y, 200, 20, 2, 10);
		}
	}
	for (int i = y - h / 2; i <= y + h / 2; i++) {
		int sss = to_string(int(i / 10.0)).size() * 10;
		if (i % 20 == 0 && i != 0) {
			g.drawText(to_string(int(i / 10.0)), w / 2 - x - 205, h / 2 + y - i - 10, 200, 20, 1, 10);
		}
	}
	g.color(0, 0, 255);
	g.drawLine(mouse_x, mouse_y - 5, mouse_x, mouse_y + 5);
	g.drawLine(mouse_x - 5, mouse_y, mouse_x + 5, mouse_y);
	g.drawText("(" + tostr((x - w / 2 + mouse_x) / 10.0) + "," + tostr((y + h / 2 - mouse_y) / 10.0) + ")",
	           mouse_x - 100, mouse_y - 15, 200, 8, 2, 10);
	g.color(200, 200, 200);
	g.drawLine(mouse_x, 0, mouse_x, h, 0.5f);
	g.color(255, 0, 0);
	for (int i = x - w / 2; i <= x + w / 2; i++) {
		make(i / 10.0);
	}
	for (int i = x - w / 2; i < x + w / 2; i++) {
		g.drawLine(i + w / 2 - x, h / 2 - ans[i / 10.0] * 10 + y, i + 1 + w / 2 - x, h / 2 - ans[(i + 1) / 10.0] * 10 + y);
		if (i + w / 2 - x == mouse_x && abs(mouse_y - (h / 2 - ans[i / 10.0] * 10 + y)) > 20) {
			g.drawText("(" + tostr(i / 10.0) + "," + tostr(ans[i / 10.0]) + ")",
			           mouse_x - 100, h / 2 - ans[i / 10.0] * 10 + y - 10, 200, 8, 2, 10);
		}
	}
}
void Window::createFinished(HWND hwnd) {}
bool Window::defaultMsg(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	return false;
}

int main() {
	initGraphics();
	string title;
	while (1) {
		system("cls");
		cout << "选择函数类型：\n";
		cout << " 1. 一次函数\n";
		cout << " 2. 二次函数\n";
		cout << " 3. 反比例函数\n";
		int ch = getch();
		if (ch == '1') {
			system("cls");
			cout << "一次函数形式：y=kx+b\n";
			double k, b;
			cout << "请输入k的值：";
			cin >> k;
			cout << "请输入b的值：";
			cin >> b;
			title = "一次函数";
			make = [k, b](double x) {
				if (ans.find(x) == ans.end()) {
					ans[x] = k * x + b;
				}
			};
			break;
		} else if (ch == '2') {
			system("cls");
			cout << "二次函数形式：y=ax^2+bx+c\n";
			double a, b, c;
			cout << "请输入a的值：";
			cin >> a;
			cout << "请输入b的值：";
			cin >> b;
			cout << "请输入c的值：";
			cin >> c;
			title = "二次函数";
			make = [a, b, c](double x) {
				if (ans.find(x) == ans.end()) {
					ans[x] = a * x * x + b * x + c;
				}
			};
			break;
		} else if (ch == '3') {
			system("cls");
			cout << "反比例函数形式：y=k/x\n";
			double k;
			cout << "请输入k的值：";
			cin >> k;
			title = "反比例函数";
			make = [k](double x) {
				if (ans.find(x) == ans.end()) {
					ans[x] = k / x;
				}
			};
			break;
		}
	}
	ShowWindow(GetConsoleWindow(), SW_HIDE);
	window.create(600, 480, title);
	cleanGraphics();
	return 0;
}
