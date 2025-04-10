#include <iostream>
#include "INC_Windows.h"
#include "GeoShapeManager.h"

namespace
{
	class DebugConsole
	{
		DebugConsole(const DebugConsole&) = delete;
		DebugConsole& operator=(const DebugConsole&) = delete;
		DebugConsole(DebugConsole&&) = delete;
		DebugConsole& operator=(DebugConsole&&) = delete;
	public:
		DebugConsole()
		{
			AllocConsole();

			FILE* stream;
			freopen_s(&stream, "CONOUT$", "w", stdout);
			freopen_s(&stream, "CONIN$", "r", stdin);
		}

		~DebugConsole()
		{
			fclose(stdout);
			fclose(stdin);

			FreeConsole();
		}

		static void Show()
		{
			::ShowWindow(GetConsoleWindow(), SW_SHOW);
		}

		static void Hide()
		{
			::ShowWindow(GetConsoleWindow(), SW_HIDE);
		}

		static void SetTitle(const wchar_t* title)
		{
			::SetConsoleTitle(title);
		}

		static void ToggleShow()
		{
			HWND hwnd = GetConsoleWindow();
			if (hwnd)
			{
				if (::IsWindowVisible(hwnd))
				{
					Hide();
				}
				else
				{
					Show();
				}
			}
		}
	};

	DebugConsole g_debugConsole;
}
namespace
{
	LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		switch (msg)
		{
		case WM_CREATE: //windows message ������ ������
		{
			std::cout << "WM_CREATE" << std::endl;
		}break;

		case WM_CLOSE: //������ �����
		{
			std::cout << "WM_CLOSE" << std::endl;
			// �����츦 �����ϱ� ���� �޽���
			PostQuitMessage(0); // WM_QUIT
		}break;
		case WM_KEYDOWN:
		{
			if (wparam == 'C' || wparam == 'c')
			{
				DebugConsole::ToggleShow();
			}
		}break;
		case WM_PAINT:
		{
			std::cout << "WM_PAINT" << std::endl;

			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd, &ps);

			RECT clientRect;
			GetClientRect(hwnd, &clientRect);
			FillRect(hdc, &clientRect, (HBRUSH)(COLOR_WINDOW + 1));

			simplegeo::g_GeoShapeManager.Draw(hdc);

			EndPaint(hwnd, &ps);

		}break;
		case WM_LBUTTONDOWN:
		{
			std::cout << "WM_LBUTTONDBLCLK" << std::endl;
			// ��ǥ�� �����´�.
			int x = LOWORD(lparam);
			int y = HIWORD(lparam);

			simplegeo::g_GeoShapeManager.AddCircle(x, y, 10, RGB(255, 0, 0));

			::InvalidateRect(hwnd, NULL, TRUE);

		}break;
		case WM_RBUTTONDOWN:
		{
			std::cout << "WM_RBUTTONDBLCLK" << std::endl;
			// ��ǥ�� �����´�.
			int x = LOWORD(lparam);
			int y = HIWORD(lparam);

			simplegeo::g_GeoShapeManager.AddRectangle(x - 10, y - 10, x + 10, y + 10, RGB(0, 255, 0));

			::InvalidateRect(hwnd, NULL, TRUE);

		}break;
		default:
			return::DefWindowProc(hwnd, msg, wparam, lparam);
		}
		return NULL;
	}
	HWND CreateWidow(const wchar_t* className, const wchar_t* windowName, int width, int height)
	{
		WNDCLASSEX wc = {};
		wc.cbSize = sizeof(WNDCLASSEX);
		wc.lpszClassName = className;
		wc.lpfnWndProc = &WndProc; // ������ ���ν���(�Լ�)�� ������ ���


		ATOM classId = 0;
		if (!GetClassInfoEx(HINSTANCE(), L"NzWindowClass", &wc))
		{
			classId = RegisterClassEx(&wc); // �ü������ ���� ���� Ŭ���� ������ ������ '������ ��� ����'�ϳ� �������

			if (0 == classId) return NULL;

			std::cout << "classId: " << classId << std::endl;
		}

		RECT rc = { 0, 0, width, height };

		AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, false); // â ũ�� ����

		std::cout << "width: " << rc.right - rc.left << " height: " << rc.bottom - rc.top << std::endl;
		// ���� WNDCLASSEX wc �� �ü���� ����ؼ� ���� �ĺ�Ű�� ������  (classId) ������ (Ŀ��) ��ü�� ����� �޶� �ϰ� ������ �ڵ��� �޾ƿɴϴ�.
		HWND hWnd = CreateWindowEx(NULL, MAKEINTATOM(classId), L"", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
			rc.right - rc.left, rc.bottom - rc.top, HWND(), HMENU(), HINSTANCE(), NULL);

		if (NULL == hWnd) return (HWND)(NULL); // ����

		::SetWindowText((HWND)hWnd, windowName); // ������â�� �ؽ�Ʈ ����

		ShowWindow((HWND)hWnd, SW_SHOW);  // ������ �����ֱ�
		UpdateWindow((HWND)hWnd); // ������ ������Ʈ

		return hWnd;
	}
	void GameLoop()
	{
		MSG msg = {};
		while (WM_QUIT != msg.message)
		{
			if (PeekMessage(&msg, HWND(), 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				///HERE: GAME LOGIC UPATE & RENDERING
			}
		}
	}
}

int main()
{
	// GUI â ����
	HWND hWnd = CreateWidow(L"NzWindowClass", L"����ĥ��", 1280, 720);

	if (NULL == hWnd)
	{
		std::cout << "������ ���� ����" << std::endl;
		system("pause");
		return -1;
	}

	std::cout << "hWnd: " << hWnd << std::endl;
	//std::cout << (*hWnd).unused << std::endl; ��Ÿ�� ����!

	GameLoop();

	//system("pause");

	DestroyWindow(hWnd);

	return 0;
}