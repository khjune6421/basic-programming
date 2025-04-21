#include <cassert>
#include <math.h>
#include "GameWindow.h"
#include "Time.h"

LARGE_INTEGER frequency;
LARGE_INTEGER currentTime;
LARGE_INTEGER previousTime;

double gameTime = 0.0;
double deltaTime = 0.0;

static void Timer()
{
	QueryPerformanceCounter(&currentTime);

	deltaTime = (currentTime.QuadPart - previousTime.QuadPart) * 1000.0 / frequency.QuadPart;
	gameTime += deltaTime;

	previousTime = currentTime;
}

void Game::Run()
{
	MSG msg = { 0 };
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			if (GetAsyncKeyState(VK_DOWN) & 0x8000)
			{
				std::cout << "down";
				input = 2;
			}
			else if (GetAsyncKeyState(VK_UP) & 0x8000)
			{
				std::cout << "up";
				input = 1;
			}
			else
			{
				input = 0;
			}
			Update();
			Render();
		}
	}
}

bool Game::Initialize()
{  
   QueryPerformanceFrequency(&frequency);
   QueryPerformanceCounter(&previousTime);

   const wchar_t* className = L"FlappyDino";
   const wchar_t* windowName = L"FlappyDino";

   if (false == __super::Create(className, windowName, 1920, 1080))
   {
       std::cout << "윈도우 생성 실패" << std::endl;
       return false;
   }

   RECT rcClient = {};
   GetClientRect(m_hWnd, &rcClient);
   m_width = rcClient.right - rcClient.left;
   m_height = rcClient.bottom - rcClient.top;

   m_hFrontDC = GetDC(m_hWnd);
   m_hBackDC = CreateCompatibleDC(m_hFrontDC);
   m_hBackBitmap = CreateCompatibleBitmap(m_hFrontDC, m_width, m_height);
   m_hDefaultBitmap = (HBITMAP)SelectObject(m_hBackDC, m_hBackBitmap);
   m_gameObjectPtrTable = new GameObject * [MAX_GAME_OBJECT_COUNT];
   m_pPlayerBitmapInfo = CreateBitmapInfo(L"../Resource/redbird.png");
   m_pEnemyBitmapInfo = CreateBitmapInfo(L"../Resource/graybird.png");

   if (m_pPlayerBitmapInfo == nullptr || m_pEnemyBitmapInfo == nullptr)
   {
	   std::cout << "Bitmap Load Failed!" << std::endl;
	   return false;
   }
   std::cout << "Bitmap Load Success!" << std::endl;

   for (int i = 0; i < MAX_GAME_OBJECT_COUNT; ++i)
   {
	   m_gameObjectPtrTable[i] = nullptr;
   }

   CreatePlayer();

   return true;
}

void Game::Finalize()
{
	if (m_gameObjectPtrTable)
	{
		for (int i = 0; i < MAX_GAME_OBJECT_COUNT; ++i)
		{
			if (m_gameObjectPtrTable[i])
			{
				delete m_gameObjectPtrTable[i];
				m_gameObjectPtrTable[i] = nullptr;
			}
		}
		delete m_gameObjectPtrTable;
	}

	__super::Destroy();
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)
	{
	case WM_SIZE:
	{
		WindowBase* pWindow = (WindowBase*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
		if (pWindow)pWindow->OnResize(LOWORD(lparam), HIWORD(lparam));
		break;
	}

	case WM_CLOSE:
	{
		WindowBase* pWindow = (WindowBase*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
		if (pWindow) pWindow->OnClose();
		PostQuitMessage(0);
		break;
	}
	default:
		return::DefWindowProc(hwnd, msg, wparam, lparam);
	}
	return NULL;
}

bool WindowBase::Create(const wchar_t* className, const wchar_t* windowName, int width, int height)
{
	WNDCLASSEX wc = {};

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.lpszClassName = className;
	wc.lpfnWndProc = WindowProc;

	ATOM classId = 0;

	if (!GetClassInfoEx(HINSTANCE(), className, &wc))
	{
		classId = RegisterClassEx(&wc);
		if (0 == classId)
		{
			std::cout << "classId가 0임" << GetLastError() << std::endl;
			return false;
		}
	}

	m_width = width;
	m_height = height;

	RECT rc = { 0, 0, width, height };

	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, false);

	m_hWnd = CreateWindowEx(NULL, MAKEINTATOM(classId), L"", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, HWND(), HMENU(), HINSTANCE(), NULL);
	
	if (NULL == m_hWnd)
	{
		std::cout << "m_hWnd가 NULL임" << GetLastError() << std::endl;
		return false;
	}

	::SetWindowText((HWND)m_hWnd, windowName);
	SetWindowLongPtr(m_hWnd, GWLP_USERDATA, (LONG_PTR)this);

	ShowWindow(m_hWnd, SW_SHOW);
	UpdateWindow(m_hWnd);

	return true;
}

void WindowBase::Destroy()
{
	if (m_hWnd)
	{
		DestroyWindow(m_hWnd);
		m_hWnd = nullptr;
	}
}

void WindowBase::OnResize(int width, int height)
{
	m_width = width;
	m_height = height;

	RECT rc = { 0, 0, width, height };

	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, false);
	SetWindowPos(m_hWnd, HWND_TOP, 0, 0, rc.right - rc.left, rc.bottom - rc.top, SWP_NOMOVE);
}

void Game::CreatePlayer()
{
	assert(m_gameObjectPtrTable[0] == nullptr && "Player object already exists!");

	Player* pPlayerObject = new Player(ObjectType::PLAYER);

	pPlayerObject->SetPosition(100.0f, DEFALT_HIGHT);
	pPlayerObject->SetCollider(100.0f, 100.0f);
	pPlayerObject->SetWidth(100);
	pPlayerObject->SetHeight(100);
	pPlayerObject->SetBitmapInfo(m_pPlayerBitmapInfo);

	m_gameObjectPtrTable[0] = pPlayerObject;
}

void Game::CreateEnemy()
{
	Enemy* pNewEnemy = new Enemy(ObjectType::ENEMY);

	float x = m_EnemySpawnPos.x;
	float y = m_EnemySpawnPos.y;

	pNewEnemy->SetPosition(x, y);
	pNewEnemy->SetSpeed(-0.5f);
	pNewEnemy->SetCollider(100.0f, 100.0f);
	pNewEnemy->SetWidth(100);
	pNewEnemy->SetHeight(100);
	pNewEnemy->SetBitmapInfo(m_pEnemyBitmapInfo);

	int i = 0;
	while (++i < MAX_GAME_OBJECT_COUNT) //0번째는 언제나 플레이어!
	{
		if (m_gameObjectPtrTable[i] == nullptr)
		{
			m_gameObjectPtrTable[i] = pNewEnemy;
			break;
		}
	}
	if (i == MAX_GAME_OBJECT_COUNT)
	{
		delete pNewEnemy;
		pNewEnemy = nullptr;
	}
}

void Game::PlayerMovement(short input)
{
	Player* pPlayer = (Player*)m_gameObjectPtrTable[0];
	float pPosition = pPlayer->GetPosition().y;

	jumpTime += deltaTime;

	if (pPosition >= DEFALT_HIGHT - 0.1)
	{
		switch (input)
		{
		case 0:
			pPlayer->SetPositionY(DEFALT_HIGHT);
			jumpTime = 0.0;
			break;
		case 1:
			pPlayer->SetPositionY(pPosition - (JUMP_POWER * deltaTime));
			isJumping = true;
			jumpTime = 0.0;
			break;
		case 2:
			pPlayer->SetPositionY(DEFALT_HIGHT);
			jumpTime = 0.0;
			break;
		}
	}
	else if (isJumping)
	{
		switch (input)
		{
		case 0:
			pPlayer->SetPositionY(pPosition + (GRAVITY * pow(jumpTime, 2.0)));
			isJumping = false;
			break;
		case 1:
			pPlayer->SetPositionY(pPosition - (JUMP_POWER * deltaTime) + (GRAVITY * pow(jumpTime, 2.0)));
			break;
		case 2:
			pPlayer->SetPositionY(pPosition + ((JUMP_POWER / 2) * pow(deltaTime, 2.0) + (GRAVITY * pow(jumpTime, 2.0))));
			isJumping = false;
			break;
		}
	}
	else
	{
		switch (input)
		{
		case 0:
			pPlayer->SetPositionY(pPosition + (GRAVITY * pow(jumpTime, 2.0)));
			break;
		case 1:
			pPlayer->SetPositionY(pPosition - (JUMP_POWER * deltaTime) + (GRAVITY * pow(jumpTime, 2.0)));
			break;
		case 2:
			pPlayer->SetPositionY(pPosition + ((JUMP_POWER / 2) * deltaTime) + (GRAVITY * pow(jumpTime, 2.0)));
			break;
		}
	}
}

void Game::Update()
{
	static int creatEnemyLoop = 1;

	Timer();
	LogicUpdate();

	while (gameTime >= (2000 * creatEnemyLoop))
	{
		CreateEnemy();
		creatEnemyLoop++;
	}
}

void Game::LogicUpdate()
{
	PlayerMovement(input);
	Player* pPlayer = (Player*)m_gameObjectPtrTable[0];
	pPlayer->Update(deltaTime);
	for (int i = 1; i < MAX_GAME_OBJECT_COUNT; ++i)
	{
		if (m_gameObjectPtrTable[i])
		{
			Enemy* pEnemy = (Enemy*)m_gameObjectPtrTable[i];
			pEnemy->Update(deltaTime);
		}
	}
}

void Game::Render()
{
	//std::cout << "rendering";
	bool isColliding = false;

	::PatBlt(m_hBackDC, 0, 0, m_width, m_height, WHITENESS);
	Player* pPlayer = (Player*)m_gameObjectPtrTable[0];
	Collider* pPlayerCollision = pPlayer->GetCollider();
	for (int i = 1; i < MAX_GAME_OBJECT_COUNT; ++i)
	{
		Enemy* pEnemy = (Enemy*)m_gameObjectPtrTable[i];
		if (pEnemy != nullptr)
		{
			pEnemy->UpdateFrame(deltaTime);
		}
		if (m_gameObjectPtrTable[i])
		{
			Collider* pEnemyCollision = pEnemy->GetCollider();
			if (Intersect(*pPlayerCollision, *pEnemyCollision))
			{
				pEnemy->Render(m_hBackDC, true);
				isColliding = true;
			}
			else
			{
				pEnemy->Render(m_hBackDC, false);
			}
		}
	}
	pPlayer->UpdateFrame(deltaTime);
	pPlayer->Render(m_hBackDC, isColliding);
	BitBlt(m_hFrontDC, 0, 0, m_width, m_height, m_hBackDC, 0, 0, SRCCOPY);
}