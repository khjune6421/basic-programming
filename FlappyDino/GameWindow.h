#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>
#include <iostream>
#include "GameObject.h"
#include "Render.h"

class GameObject;
class BitmapInfo;

constexpr int MAX_GAME_OBJECT_COUNT = 100;
constexpr float DEFALT_HIGHT = 1000.0f;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

class WindowBase
{
public:

	WindowBase() = default;
	virtual ~WindowBase() = default;
	bool Create(const wchar_t* className, const wchar_t* windowName, int width, int height);
	void Destroy();
	void* GetHandle() const { return m_hWnd; }
	int GetWidth() const { return m_width; }
	int GetHeight() const { return m_height; }

protected:

	friend LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	virtual void OnResize(int width, int height);
	virtual void OnClose() {}
	HWND m_hWnd = HWND();

	int m_width = 100;
	int m_height = 100;

	WindowBase(const WindowBase&) = delete;
	WindowBase& operator=(const WindowBase&) = delete;
	WindowBase(WindowBase&&) = delete;
	WindowBase& operator=(WindowBase&&) = delete;
};

class Game : public WindowBase
{
public:
	Game() = default;
	~Game() = default;
	void Run();
	bool Initialize();
	void Finalize();

private:
	HDC m_hFrontDC = nullptr;
	HDC m_hBackDC = nullptr;
	HBITMAP m_hBackBitmap = nullptr;
	HBITMAP m_hDefaultBitmap = nullptr;
	GameObject** m_gameObjectPtrTable = nullptr;

	void Update();
	void Render();
	void LogicUpdate();
	void CreatePlayer();
	void CreateEnemy();
	void PlayerMovement(short input);

	short input = 0;
	const float JUMP_POWER = 0.5f;
	const float GRAVITY = 0.000001f;
	double jumpTime = 0.0;
	bool isJumping = false;

	Vector m_EnemySpawnPos = { 2000, DEFALT_HIGHT };

	using BitmapInfo = BitmapInfo;
	BitmapInfo* m_pPlayerBitmapInfo = nullptr;
	BitmapInfo* m_pEnemyBitmapInfo = nullptr;
};