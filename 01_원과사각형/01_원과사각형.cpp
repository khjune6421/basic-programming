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
		case WM_CREATE: //windows message 윈도우 생성시
		{
			std::cout << "WM_CREATE" << std::endl;
		}break;

		case WM_CLOSE: //원도우 종료시
		{
			std::cout << "WM_CLOSE" << std::endl;
			// 윈도우를 종료하기 위한 메시지
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
			// 좌표를 가져온다.
			int x = LOWORD(lparam);
			int y = HIWORD(lparam);

			simplegeo::g_GeoShapeManager.AddCircle(x, y, 10, RGB(255, 0, 0));

			::InvalidateRect(hwnd, NULL, TRUE);

		}break;
		case WM_RBUTTONDOWN:
		{
			std::cout << "WM_RBUTTONDBLCLK" << std::endl;
			// 좌표를 가져온다.
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
		wc.lpfnWndProc = &WndProc; // 윈도우 프로시저(함수)의 포인터 등록


		ATOM classId = 0;
		if (!GetClassInfoEx(HINSTANCE(), L"NzWindowClass", &wc))
		{
			classId = RegisterClassEx(&wc); // 운영체제한테 짐금 만든 클래스 정보를 가지고 '윈도우 등록 정보'하나 만들어줘

			if (0 == classId) return NULL;

			std::cout << "classId: " << classId << std::endl;
		}

		RECT rc = { 0, 0, width, height };

		AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, false); // 창 크기 조절

		std::cout << "width: " << rc.right - rc.left << " height: " << rc.bottom - rc.top << std::endl;
		// 만든 WNDCLASSEX wc 를 운영체제에 등록해서 받은 식별키를 가지고  (classId) 윈도우 (커널) 객체를 만들어 달라 하고 윈도우 핸들을 받아옵니다.
		HWND hWnd = CreateWindowEx(NULL, MAKEINTATOM(classId), L"", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
			rc.right - rc.left, rc.bottom - rc.top, HWND(), HMENU(), HINSTANCE(), NULL);

		if (NULL == hWnd) return (HWND)(NULL); // 실패

		::SetWindowText((HWND)hWnd, windowName); // 원도우창에 텍스트 설정

		ShowWindow((HWND)hWnd, SW_SHOW);  // 윈도우 보여주기
		UpdateWindow((HWND)hWnd); // 윈도우 업데이트

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
	// GUI 창 생성
	HWND hWnd = CreateWidow(L"NzWindowClass", L"원과 사각형", 1280, 720);

	if (NULL == hWnd)
	{
		std::cout << "윈도우 생성 실패" << std::endl;
		system("pause");
		return -1;
	}

	std::cout << "hWnd: " << hWnd << std::endl;
	//std::cout << (*hWnd).unused << std::endl; 런타임 에러!

	GameLoop();

	//system("pause");

	DestroyWindow(hWnd);

	return 0;
}