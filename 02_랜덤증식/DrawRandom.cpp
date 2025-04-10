#include "DrawRandom.h"
#include "INC_Windows.h"
#include "GameTimer.h"
#include <random>
#include "GeoShapeManager.h"
#include <iostream>

std::random_device rd;
unsigned long long seed = rd();
std::mt19937 gen(seed);

void DrawRandom::GameInit()
{
	RECT rcClient = {};
	GetClientRect(hWnd, &rcClient);

	const int circleRadius = 10;
	const int& clientWidth = rcClient.right - rcClient.left;
	const int& clientHeight = rcClient.bottom - rcClient.top;

	//simplegeo::g_GeoShapeManager.AddCircle(clientWidth / 2, clientHeight / 2, circleRadius, RGB(255, 0, 0));
}

void DrawRandom::GameLoop()
{
	GameTimer timer;
	timer.Reset();

	MSG msg = {};
	while (WM_QUIT != msg.message)
	{
		timer.Tick();
		if (PeekMessage(&msg, HWND(), 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else if (GameUpdate(timer.DeltaTimeMS()))
		{
			GameRender();
		}
	}
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)
	{
	case WM_CREATE:
	{
		std::cout << "WM_CREATE" << std::endl;
	}break;

	case WM_CLOSE:
	{
		std::cout << "WM_CLOSE" << std::endl;

		PostQuitMessage(0); // WM_QUIT
	}break;
	default:
		return::DefWindowProc(hwnd, msg, wparam, lparam);
	}
	return NULL;
}

HWND DrawRandom::CreateWidow(const wchar_t* className, const wchar_t* windowName, int width, int height)
{
	WNDCLASSEX wc = {};
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.lpszClassName = className;
	wc.lpfnWndProc = WndProc; // 윈도우 프로시저(함수)의 포인터 등록


	ATOM classId = 0;
	if (!GetClassInfoEx(HINSTANCE(), L"NzWindowClass", &wc))
	{
		classId = RegisterClassEx(&wc);

		if (0 == classId) return NULL;
	}

	RECT rc = { 0, 0, width, height };

	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, false);

	std::cout << "width: " << rc.right - rc.left << " height: " << rc.bottom - rc.top << std::endl;

	HWND hWnd = CreateWindowEx(NULL, MAKEINTATOM(classId), L"", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
		rc.right - rc.left, rc.bottom - rc.top, HWND(), HMENU(), HINSTANCE(), NULL);

	if (NULL == hWnd) return (HWND)(NULL);

	::SetWindowText((HWND)hWnd, windowName);

	ShowWindow((HWND)hWnd, SW_SHOW);
	UpdateWindow((HWND)hWnd);

	return hWnd;
}

void DrawRandom::GameRender()
{
	RECT rcClient = {};
	GetClientRect(hWnd, &rcClient);

	const int circleRadius = 20;
	std::uniform_int_distribution<int> circleX(0, rcClient.right - rcClient.left);
	std::uniform_int_distribution<int> circleY(0, rcClient.bottom - rcClient.top);
	std::uniform_int_distribution<int> circleOrRectangle(0, 1);

	int circleXLocation = circleX(gen);
	int circleYLocation = circleY(gen);
	int isCircle = circleOrRectangle(gen);

	if (isCircle)
	{
		simplegeo::g_GeoShapeManager.AddCircle(circleXLocation, circleYLocation, circleRadius, RGB(255, 0, 0));
	}
	else
	{
		simplegeo::g_GeoShapeManager.AddRectangle(circleXLocation - 20, circleYLocation - 20, circleXLocation + 20, circleYLocation + 20, RGB(0, 0, 255));
	}

	GetClientRect(hWnd, &clientRect);

	HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);

	FillRect(hMemDC, &clientRect, hBrush);

	simplegeo::g_GeoShapeManager.Draw(hMemDC);

	BitBlt(hDC, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, hMemDC, 0, 0, SRCCOPY);

	SelectObject(hMemDC, hOldBitmap);
}

bool DrawRandom::GameUpdate(float deltaTime)
{
	acculatedTime += deltaTime;

	if (acculatedTime < 1000.0f)
	{
		return false;
	}
	else
	{
		acculatedTime -= 1000.0f;
		return true;
	}
}
