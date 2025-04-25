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
#include <random>
#include "GameObject.h"
#include "Render.h"

class GameObject;
class BitmapInfo;

constexpr int MAX_GAME_OBJECT_COUNT = 100;
constexpr float DEFALT_HIGHT = 450.0f;
constexpr int MAIN_SCENE_TEXT_SIZE = 38;
constexpr int END_SCENE_TEXT_SIZE1 = 10;
constexpr int END_SCENE_TEXT_SIZE2 = 11;

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
	void RenderScore();
	void LogicUpdate();
	void CreatePlayer();
	void CreateEnemy();
	void PlayerMovement(short input);

	const float DEFALT_PLAYER_COLLIDER_Y = 100.0f;
	short input = 0;
	float jumpPower = 1.2f;
	const double GRAVITY = 300.0;
	double jumpAndGravity = 0.0;
	double jumpTime = 0.0;
	bool isDucking = false;
	int gameScore = 0;
	int hiScore = 0;
	char hiScoreText[6] = "00000";
	char scoreText[6] = "00000";
	bool isColliding = false;
	char mainSceneText[MAIN_SCENE_TEXT_SIZE] = "Press Space Or Arrow Key Up To Start!";
	char endSceneText1[END_SCENE_TEXT_SIZE1] = "Game Over";
	char endSceneText2[END_SCENE_TEXT_SIZE2] = "Try Again?";
	int creatEnemyLoop = 1;
	bool isKeyPressed = false;

	Vector m_EnemySpawnPos = { 2000, DEFALT_HIGHT };
	std::mt19937 gameGen;
	enum Scene
	{
		MAIN_SCENE,
		GAME_SCENE,
		END_SCENE
	};
	Scene scene = MAIN_SCENE;

	void MainSceneRender();
	void EndSceneRender();

	using BitmapInfo = BitmapInfo;
	BitmapInfo* m_pPlayerBitmapInfoRun = nullptr;
	BitmapInfo* m_pPlayerBitmapInfoDuck = nullptr;
	BitmapInfo* m_pPlayerBitmapInfoDeath = nullptr;
	BitmapInfo* m_pEnemyBitmapInfoCactus = nullptr;
	BitmapInfo* m_pEnemyBitmapInfoBird = nullptr;
};