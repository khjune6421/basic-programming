#include <cassert>
#include <math.h>
#include <vector>
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
			if ((GetAsyncKeyState(VK_UP) & 0x8000 | (GetAsyncKeyState(VK_SPACE) & 0x8000)) == false)
			{
				isKeyPressed = false;
			}
			if (GetAsyncKeyState(VK_DOWN) & 0x8000)
			{
				input = 2;
			}
			else if (isKeyPressed == false && (GetAsyncKeyState(VK_UP) & 0x8000 || (GetAsyncKeyState(VK_SPACE) & 0x8000)))
			{
				input = 1;
				isKeyPressed = true;
				if (scene == MAIN_SCENE)
				{
					gameTime = 0.0;
					creatEnemyLoop = 1;
					scene = GAME_SCENE;
					for (int i = 1; i < MAX_GAME_OBJECT_COUNT; ++i)
					{
						if (m_gameObjectPtrTable[i])
						{
							delete m_gameObjectPtrTable[i];
							m_gameObjectPtrTable[i] = nullptr;
						}
					}
				}
			}
			else if (GetAsyncKeyState(VK_UP) & 0x8000 || (GetAsyncKeyState(VK_SPACE) & 0x8000))
			{
				input = 3;
				isKeyPressed = true;
			}
			else
			{
				input = 0;
			}
			if (scene == GAME_SCENE)
			{
				Update();
				Render();
			}
			else if (scene == MAIN_SCENE)
			{
				Timer();
				MainSceneRender();
			}
			else if (scene == END_SCENE)
			{
				EndSceneRender();
				if (isKeyPressed == false && (GetAsyncKeyState(VK_UP) & 0x8000 || (GetAsyncKeyState(VK_SPACE) & 0x8000)))
				{
					isKeyPressed = true;
					scene = MAIN_SCENE;
				}
			}
		}
	}
}

bool Game::Initialize()
{
	std::random_device rd;
	unsigned long long seed = rd();
	std::mt19937 gen(static_cast<unsigned int>(seed));
	gameGen = gen;

	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&previousTime);

	const wchar_t* className = L"FlappyDino";
	const wchar_t* windowName = L"FlappyDino";

	if (false == __super::Create(className, windowName, 1280, 540))
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
	m_pPlayerBitmapInfoRun = CreateBitmapInfo(L"../Resource/dino_run.png");
	m_pPlayerBitmapInfoDuck = CreateBitmapInfo(L"../Resource/dino_lower.png");
	m_pPlayerBitmapInfoDeath = CreateBitmapInfo(L"../Resource/dino_death.png");
	m_pEnemyBitmapInfoCactus = CreateBitmapInfo(L"../Resource/small_cactuses.png");
	m_pEnemyBitmapInfoBird = CreateBitmapInfo(L"../Resource/pterosaur.png");

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
	pPlayerObject->SetCollider(100.0f, DEFALT_PLAYER_COLLIDER_Y);
	pPlayerObject->SetWidth(100);
	pPlayerObject->SetHeight(100);
	pPlayerObject->SetBitmapInfo(m_pPlayerBitmapInfoRun);

	m_gameObjectPtrTable[0] = pPlayerObject;
}

void Game::CreateEnemy()
{
	std::uniform_int_distribution<int> dist1(0, 1);
	std::uniform_int_distribution<int> dist2(1, 2);
	int enemySpawnRandom = dist1(gameGen);
	int enemyBirdSpawnRandom = dist2(gameGen);

	Enemy* pNewEnemy = new Enemy(ObjectType::ENEMY);

	float x = m_EnemySpawnPos.x;
	float y = m_EnemySpawnPos.y;

	pNewEnemy->SetPosition(x, y - (70 * enemySpawnRandom * enemyBirdSpawnRandom));
	pNewEnemy->SetSpeed(-0.5f - static_cast <float>(gameTime / 30000.0));
	pNewEnemy->SetCollider(100.0f, 100.0f);
	pNewEnemy->SetWidth(100);
	pNewEnemy->SetHeight(100);
	if (enemySpawnRandom)
	{
		pNewEnemy->SetBitmapInfo(m_pEnemyBitmapInfoBird);
	}
	else
	{
		pNewEnemy->SetBitmapInfo(m_pEnemyBitmapInfoCactus);
	}

	int i = 0;
	while (++i < MAX_GAME_OBJECT_COUNT)
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

	isDucking = false;
	jumpTime += deltaTime;
	jumpAndGravity = jumpPower - (jumpTime / (GRAVITY / 1.4));

	if (pPosition >= DEFALT_HIGHT - 0.1)
	{
		jumpPower = 1.2f;
		switch (input)
		{
		case 0:
			pPlayer->SetPositionY(DEFALT_HIGHT);
			jumpTime = 0.0;
			break;
		case 1:
			pPlayer->SetPositionY(pPosition - static_cast <float>(jumpAndGravity * deltaTime));
			jumpTime = 0.0;
			break;
		case 2:
			pPlayer->SetPositionY(DEFALT_HIGHT);
			jumpTime = 0.0;
			isDucking = true;
			break;
		case 3:
			pPlayer->SetPositionY(DEFALT_HIGHT);
			jumpTime = 0.0;
			break;
		}
	}
	else if (jumpTime < 200.0)
	{
		switch (input)
		{
		case 0:
			pPlayer->SetPositionY(pPosition - static_cast <float>(jumpAndGravity * deltaTime));
			break;
		case 1:
			pPlayer->SetPositionY(pPosition - static_cast <float>(jumpAndGravity * deltaTime));
			break;
		case 2:
			if (jumpAndGravity > 0.0)
			{
				jumpTime += GRAVITY;
			}
			pPlayer->SetPositionY(pPosition + ((jumpPower / 2) * static_cast <float>(deltaTime)) - static_cast <float>(jumpAndGravity * deltaTime));
			break;
		case 3:
			pPlayer->SetPositionY(pPosition - static_cast <float>(jumpAndGravity * deltaTime));
			jumpPower += 0.003f * static_cast <float>(deltaTime);
			break;
		}
	}
	else
	{
		switch (input)
		{
		case 0:
			pPlayer->SetPositionY(pPosition - static_cast <float>(jumpAndGravity * deltaTime));
			break;
		case 1:
			pPlayer->SetPositionY(pPosition - static_cast <float>(jumpAndGravity * deltaTime));
			break;
		case 2:
			if (jumpAndGravity > 0.0)
			{
				jumpTime += GRAVITY;
			}
			pPlayer->SetPositionY(pPosition + ((jumpPower / 2) * static_cast <float>(deltaTime)) - static_cast <float>(jumpAndGravity * deltaTime));
			break;
		case 3:
			pPlayer->SetPositionY(pPosition - static_cast <float>(jumpAndGravity * deltaTime));
			break;
		}
	}
	if (isDucking)
	{
		pPlayer->SetColliderH(DEFALT_PLAYER_COLLIDER_Y / 2.0f);
		pPlayer->SetBitmapInfo(m_pPlayerBitmapInfoDuck);
	}
	else
	{
		pPlayer->SetColliderH(DEFALT_PLAYER_COLLIDER_Y);
		pPlayer->SetBitmapInfo(m_pPlayerBitmapInfoRun);
	}
}

void Game::MainSceneRender()
{
	::PatBlt(m_hBackDC, 0, 0, m_width, m_height, WHITENESS);

	Player* pPlayer = (Player*)m_gameObjectPtrTable[0];
	pPlayer->SetPosition(100.0f, DEFALT_HIGHT);
	pPlayer->SetBitmapInfo(m_pPlayerBitmapInfoRun);
	pPlayer->Update(false);
	pPlayer->UpdateFrame(deltaTime);
	pPlayer->Render(m_hBackDC, isColliding);

	TextOutA(m_hBackDC, (m_width / 2) - (MAIN_SCENE_TEXT_SIZE * 4), m_height / 2, mainSceneText, MAIN_SCENE_TEXT_SIZE - 1);
	TextOutA(m_hBackDC, 1100, 100, hiScoreText, 5);
	TextOutA(m_hBackDC, 1150, 100, scoreText, 5);

	BitBlt(m_hFrontDC, 0, 0, m_width, m_height, m_hBackDC, 0, 0, SRCCOPY);
}

void Game::EndSceneRender()
{
	::PatBlt(m_hBackDC, 0, 0, m_width, m_height, WHITENESS);
	Player* pPlayer = (Player*)m_gameObjectPtrTable[0];
	pPlayer->SetBitmapInfo(m_pPlayerBitmapInfoDeath);

	for (int i = 1; i < MAX_GAME_OBJECT_COUNT; ++i)
	{
		Enemy* pEnemy = (Enemy*)m_gameObjectPtrTable[i];
		if (pEnemy != nullptr)
		{
			pEnemy->UpdateFrame(deltaTime);
			pEnemy->Render(m_hBackDC, isColliding);
		}
	}
	pPlayer->UpdateFrame(deltaTime);
	pPlayer->Render(m_hBackDC, isColliding);
	RenderScore();

	TextOutA(m_hBackDC, (m_width / 2) - (END_SCENE_TEXT_SIZE1 * 4), m_height / 2, endSceneText1, END_SCENE_TEXT_SIZE1 - 1);
	TextOutA(m_hBackDC, (m_width / 2) - (END_SCENE_TEXT_SIZE2 * 4), (m_height / 2) + 20, endSceneText2, END_SCENE_TEXT_SIZE2 - 1);
	TextOutA(m_hBackDC, 1100, 100, hiScoreText, 5);
	TextOutA(m_hBackDC, 1150, 100, scoreText, 5);

	BitBlt(m_hFrontDC, 0, 0, m_width, m_height, m_hBackDC, 0, 0, SRCCOPY);
}

void Game::Update()
{;
	Timer();
	LogicUpdate();

	while (gameTime >= ((1500 - (creatEnemyLoop * 5)) * creatEnemyLoop))
	{
		CreateEnemy();
		creatEnemyLoop++;
	}
}

void Game::LogicUpdate()
{
	isColliding = false;
	PlayerMovement(input);
	Player* pPlayer = (Player*)m_gameObjectPtrTable[0];
	Collider* pPlayerCollider = pPlayer->GetCollider();
	pPlayer->Update(isDucking);
	for (int i = 1; i < MAX_GAME_OBJECT_COUNT; ++i)
	{
		if (m_gameObjectPtrTable[i])
		{
			Enemy* pEnemy = (Enemy*)m_gameObjectPtrTable[i];
			if (pEnemy->Update(deltaTime))
			{
				delete m_gameObjectPtrTable[i];
				m_gameObjectPtrTable[i] = nullptr;
			}
			else
			{
				Collider* pEnemyCollider = pEnemy->GetCollider();
				if (Intersect(*pPlayerCollider, *pEnemyCollider))
				{
					isColliding = true;
					isKeyPressed = true;
					scene = END_SCENE;
				}
			}
		}
	}
}

void Game::Render()
{
	//std::cout << "rendering";

	::PatBlt(m_hBackDC, 0, 0, m_width, m_height, WHITENESS);
	Player* pPlayer = (Player*)m_gameObjectPtrTable[0];
	for (int i = 1; i < MAX_GAME_OBJECT_COUNT; ++i)
	{
		Enemy* pEnemy = (Enemy*)m_gameObjectPtrTable[i];
		if (pEnemy != nullptr)
		{
			pEnemy->UpdateFrame(deltaTime);
			pEnemy->Render(m_hBackDC, isColliding);
		}
	}
	pPlayer->UpdateFrame(deltaTime);
	pPlayer->Render(m_hBackDC, isColliding);
	RenderScore();
	BitBlt(m_hFrontDC, 0, 0, m_width, m_height, m_hBackDC, 0, 0, SRCCOPY);
}

void Game::RenderScore()
{
	gameScore = static_cast <int>(gameTime / 100.0);
	if (gameScore >= hiScore)
	{
		hiScore = gameScore;
	}
	int hiScoreDigit = hiScore;
	int scoreDigit = gameScore;
	for (int i = 4; i >= 0; i--)
	{
		hiScoreText[4 - i] = (hiScoreDigit / static_cast <int>(pow(10, i))) + 48;
		hiScoreDigit = (hiScoreDigit % static_cast <int>(pow(10, i)));
		scoreText[4 - i] = (scoreDigit / static_cast <int>(pow(10, i))) + 48;
		scoreDigit = (scoreDigit % static_cast <int>(pow(10, i)));
	}
	TextOutA(m_hBackDC, 1100, 100, hiScoreText, 5);
	TextOutA(m_hBackDC, 1150, 100, scoreText, 5);
}