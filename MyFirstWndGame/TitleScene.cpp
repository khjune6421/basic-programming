#include "TitleScene.h"
#include "MyFirstWndGame.h"
#include "GameObject.h"
#include "Utillity.h"
#include <assert.h>

using namespace learning;

void TitleScene::Initialize(NzWndBase* pWnd)
{
    m_pGame = dynamic_cast<MyFirstWndGame*>(pWnd);
    assert(m_pGame != nullptr && "Game object is not initialized!");

    Background* pNewObject = new Background(ObjectType::BACKGROUND);
    pNewObject->SetPosition(0.0f, 0.0f);

    int width = m_pGame->GetWidth();
    int height = m_pGame->GetHeight();

    pNewObject->SetWidth(width);
    pNewObject->SetHeight(height);

    pNewObject->SetBitmapInfo(m_pGame->GetBackgroundBitmapInfo());

    m_rect.left = width/2 - 50;
    m_rect.top = height/2 - 50;
    m_rect.right = m_rect.left + 100;
    m_rect.bottom = m_rect.top + 100;

    m_pBackground = pNewObject;
}

void TitleScene::Update(float deltaTime)
{
   static float time = 0.0f;
    time += deltaTime;

    if (time > 10000.0f)
    {
        time = 0.0f;
        m_pGame->ChangeScene(SceneType::SCENE_PLAY);
    }
    else
    {
        wsprintf(m_szTitle, L"Title Scene %d", static_cast<int>(time / 1000.0f) + 1);
    }
}

void TitleScene::Render(HDC hDC)
{
    assert(m_pGame != nullptr && "Game object is not initialized!");
   
    m_pBackground->Render(hDC, false);
    
    DrawText(hDC, m_szTitle, -1, &m_rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
}

void TitleScene::Finalize()
{
    if (m_pBackground)
    {
        delete m_pBackground;
        m_pBackground = nullptr;
    }
}

void TitleScene::Enter()
{
   
}

void TitleScene::Leave()
{
}

