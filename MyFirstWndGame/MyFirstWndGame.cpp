#include "MyFirstWndGame.h"
#include "GameTimer.h"
#include "Collider.h"
#include "GameObject.h"
#include <iostream>
#include <assert.h>
#include "RenderHelp.h"

using namespace learning;

constexpr int MAX_GAME_OBJECT_COUNT = 1000;

bool MyFirstWndGame::Initialize()
{
    m_pGameTimer = new GameTimer();
    m_pGameTimer->Reset();

    const wchar_t* className = L"MyFirstWndGame";
    const wchar_t* windowName = L"�������Ƶ��";

    if (false == __super::Create(className, windowName, 1024, 720))
    {
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

    m_GameObjectPtrTable = new GameObjectBase * [MAX_GAME_OBJECT_COUNT];

#pragma region resource
    m_pPlayerBitmapInfo = renderHelp::CreateBitmapInfo(L"../Resource/redbird.png");
    m_pEnemyBitmapInfo = renderHelp::CreateBitmapInfo(L"../Resource/graybird.png");

    if (m_pPlayerBitmapInfo == nullptr || m_pEnemyBitmapInfo == nullptr)
    {
        std::cout << "Bitmap Load Failed!" << std::endl;
        return false;
    }
	std::cout << "Bitmap Load Success!" << std::endl;

#pragma endregion
    for (int i = 0; i < MAX_GAME_OBJECT_COUNT; ++i)
    {
        m_GameObjectPtrTable[i] = nullptr;
    }

    // [CHECK]. ù ��° ���� ������Ʈ�� �÷��̾� ĳ���ͷ� ����!
    CreatePlayer();

    return true;

}

void MyFirstWndGame::Run()
{
    MSG msg = { 0 };
    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_LBUTTONDOWN)
            {
                MyFirstWndGame::OnLButtonDown(LOWORD(msg.lParam), HIWORD(msg.lParam));
            }
            else if (msg.message == WM_RBUTTONDOWN)
            {
                MyFirstWndGame::OnRButtonDown(LOWORD(msg.lParam), HIWORD(msg.lParam));
            }
            else if (msg.message == WM_MOUSEMOVE)
            {
                MyFirstWndGame::OnMouseMove(LOWORD(msg.lParam), HIWORD(msg.lParam));
            }
            else
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        {
            Update();
            Render();
        }
    }
}

void MyFirstWndGame::Finalize()
{
    delete m_pGameTimer;
    m_pGameTimer = nullptr;

    if (m_GameObjectPtrTable)
    {
        for (int i = 0; i < MAX_GAME_OBJECT_COUNT; ++i)
        {
            if (m_GameObjectPtrTable[i])
            {
                delete m_GameObjectPtrTable[i];
                m_GameObjectPtrTable[i] = nullptr;
            }
        }
        delete m_GameObjectPtrTable;
    }

    __super::Destroy();
}

void MyFirstWndGame::FixedUpdate()
{
    if (m_EnemySpawnPos.x != 0 && m_EnemySpawnPos.y != 0)
    {
        CreateEnemy();
    }
}

void MyFirstWndGame::LogicUpdate()
{

    UpdatePlayerInfo();

    for (int i = 0; i < MAX_GAME_OBJECT_COUNT; ++i)
    {
        if (m_GameObjectPtrTable[i])
        {
            m_GameObjectPtrTable[i]->Update(m_fDeltaTime);
        }
    }
}

void MyFirstWndGame::CreatePlayer()
{
    assert(m_GameObjectPtrTable[0] == nullptr && "Player object already exists!");

    GameObject* pNewObject = new GameObject(ObjectType::PLAYER);

    pNewObject->SetName("Player");
    pNewObject->SetPosition(0.0f, 0.0f); // �ϴ�, ���Ƿ� ���� 
    pNewObject->SetSpeed(1.0f); // �ϴ�, ���Ƿ� ����   
    pNewObject->setHeight(100);
	pNewObject->setWidth(100);

    pNewObject->SetBitmapInfo(m_pPlayerBitmapInfo);
    pNewObject->SetColliderCircle(50.0f); // �ϴ�, ���Ƿ� ����. ������Ʈ ������ �� �� �ϰ� ���� ����.

    

    m_GameObjectPtrTable[0] = pNewObject;
}

void MyFirstWndGame::CreateEnemy()
{
    GameObject* pNewObject = new GameObject(ObjectType::ENEMY);

    pNewObject->SetName("Enemy");

    float x = m_EnemySpawnPos.x;
    float y = m_EnemySpawnPos.y;

    m_EnemySpawnPos = { 0, 0 };

    pNewObject->SetPosition(x, y);
    pNewObject->SetSpeed(1.0f); // �ϴ�, ���Ƿ� ���� 
    pNewObject->setHeight(100);
    pNewObject->setWidth(100);

    pNewObject->SetColliderCircle(50.0f); // �ϴ�, ���Ƿ� ����. ������Ʈ ������ �� �� �ϰ� ���� ����.
    pNewObject->SetBitmapInfo(m_pEnemyBitmapInfo);

    int i = 0;
    ColliderCircle* pNewCollisionCircle = pNewObject->GetColliderCircle();
    while (++i < MAX_GAME_OBJECT_COUNT) //0��°�� ������ �÷��̾�!
    {
        if (m_GameObjectPtrTable[i])
        {
            GameObject* pGameObject = (GameObject*)m_GameObjectPtrTable[i];
            ColliderCircle* pCollisionCircle = pGameObject->GetColliderCircle();
            if (learning::Intersect(*pNewCollisionCircle, *pCollisionCircle))
            {
				delete pNewObject;
                break;
            }
        }
        else
        {
            m_GameObjectPtrTable[i] = pNewObject;
            break;
        }
    }

    if (i == MAX_GAME_OBJECT_COUNT)
    {
        // ���� ������Ʈ ���̺��� ���� á���ϴ�.
        delete pNewObject;
        pNewObject = nullptr;
    }
}

void MyFirstWndGame::UpdatePlayerInfo()
{
    static GameObject* pPlayer = GetPlayer();

    assert(pPlayer != nullptr);

    Vector2f mousePos(m_PlayerTargetPos.x, m_PlayerTargetPos.y);
    Vector2f playerPos = pPlayer->GetPosition();

    Vector2f playerDir = mousePos - playerPos;
    float distance = playerDir.Length(); // �Ÿ� ���

    if (distance > 50.f) //���Ƿ� ������ �Ÿ�
    {
        playerDir.Normalize(); // ����ȭ
        pPlayer->SetDirection(playerDir); // �÷��̾� ���� ����
    }
    else
    {
        pPlayer->SetDirection(Vector2f(0, 0)); // �÷��̾� ����
    }
}

void MyFirstWndGame::Update()
{
    m_pGameTimer->Tick();

    LogicUpdate();

    m_fDeltaTime = m_pGameTimer->DeltaTimeMS();
    m_fFrameCount += m_fDeltaTime;

    while (m_fFrameCount >= 200.0f)
    {
        FixedUpdate();
        m_fFrameCount -= 200.0f;
    }
}

void MyFirstWndGame::Render()
{
	bool bColliding = false;
    //Clear the back buffer
    ::PatBlt(m_hBackDC, 0, 0, m_width, m_height, WHITENESS);

    //�޸� DC�� �׸���
    for (int i = 1; i < MAX_GAME_OBJECT_COUNT; ++i)
    {
        GameObject* pGameObject = (GameObject*)m_GameObjectPtrTable[i];
        if (pGameObject != nullptr)
        {
            m_fDeltaTime = m_pGameTimer->DeltaTimeMS();
            pGameObject->UpdateFrame(m_fDeltaTime);
        }
        if (m_GameObjectPtrTable[i])
        {
            GameObject* pGameObject = (GameObject*)m_GameObjectPtrTable[i];
            ColliderCircle* pCollisionCircle = pGameObject->GetColliderCircle();
            ColliderCircle* pPlayerCircle = GetPlayer()->GetColliderCircle();
            if (learning::Intersect(*pPlayerCircle, *pCollisionCircle))
            {
                m_GameObjectPtrTable[i]->Render(m_hBackDC, true);
				bColliding = true;
            }
            else
            {
                m_GameObjectPtrTable[i]->Render(m_hBackDC, false);
            }
        }
    }
    GameObject* pGameObject = (GameObject*)m_GameObjectPtrTable[0];
    m_fDeltaTime = m_pGameTimer->DeltaTimeMS();
    pGameObject->UpdateFrame(m_fDeltaTime);
    m_GameObjectPtrTable[0]->Render(m_hBackDC, bColliding);
    //�޸� DC�� �׷��� ����� ���� DC(m_hFrontDC)�� ����
    BitBlt(m_hFrontDC, 0, 0, m_width, m_height, m_hBackDC, 0, 0, SRCCOPY);
}

void MyFirstWndGame::OnResize(int width, int height)
{
    // �̼� ����� ���� �Ǹ� �÷��̾ ȭ�� �߾�����
    std::cout << __FUNCTION__ << std::endl;

    learning::SetScreenSize(width, height);

    __super::OnResize(width, height);

    m_hBackBitmap = CreateCompatibleBitmap(m_hFrontDC, m_width, m_height);

    HANDLE hPrevBitmap = (HBITMAP)SelectObject(m_hBackDC, m_hBackBitmap);

    DeleteObject(hPrevBitmap);
}

void MyFirstWndGame::OnClose()
{
    std::cout << __FUNCTION__ << std::endl;

    SelectObject(m_hBackDC, m_hDefaultBitmap);

    DeleteObject(m_hBackBitmap);
    DeleteDC(m_hBackDC);

    ReleaseDC(m_hWnd, m_hFrontDC);
}

void MyFirstWndGame::OnMouseMove(int x, int y)
{
    /*   std::cout << __FUNCTION__ << std::endl;
       std::cout << "x: " << x << ", y: " << y << std::endl;*/
    m_MousePosPrev = m_MousePos;
    m_MousePos = { x, y };
}

void MyFirstWndGame::OnLButtonDown(int x, int y)
{
    /*  std::cout << __FUNCTION__ << std::endl;
 std::cout << "x: " << x << ", y: " << y << std::endl;*/

    m_PlayerTargetPos.x = x;
    m_PlayerTargetPos.y = y;

}

void MyFirstWndGame::OnRButtonDown(int x, int y)
{
    /*  std::cout << __FUNCTION__ << std::endl;
   std::cout << "x: " << x << ", y: " << y << std::endl;*/

    m_EnemySpawnPos.x = x;
    m_EnemySpawnPos.y = y;
}