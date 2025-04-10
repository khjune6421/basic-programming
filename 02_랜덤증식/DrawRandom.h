#pragma once
#include "INC_Windows.h"

class DrawRandom
{
public:
	const int WINDOW_WIDTH = 1280;
	const int WINDOW_HEIGHT = 720;
	void GameInit();
	void GameLoop();
	HWND CreateWidow(const wchar_t* className, const wchar_t* windowName, int width, int height);
	HWND hWnd = CreateWidow(L"NzWindowClass", L"·£´ýÁõ½Ä", WINDOW_WIDTH, WINDOW_HEIGHT);
	~DrawRandom()
	{
		DeleteObject(hBitmap);
		DeleteObject(hBrush);
		DeleteDC(hMemDC);
		ReleaseDC(hWnd, hDC);
		DestroyWindow(hWnd);
	}
private:
	void GameRender();
	bool GameUpdate(float deltaTime);
	float acculatedTime = 0.0f;
	HDC hDC = GetDC(hWnd);
	HDC hMemDC = CreateCompatibleDC(hDC);
	RECT clientRect = {};
	HBITMAP hBitmap = CreateCompatibleBitmap(hDC, WINDOW_WIDTH, WINDOW_HEIGHT);
	HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 255));
};