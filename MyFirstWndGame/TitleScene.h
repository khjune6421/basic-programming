#pragma once

#include "Scene.h"

class GameObjectBase;
class MyFirstWndGame;

class TitleScene : public Scene
{
public:
    TitleScene() = default;
    ~TitleScene() override = default;

    void Initialize(NzWndBase* pWnd) override;
    void Finalize() override;

    virtual void Enter() override;
    virtual void Leave() override;

    void FixedUpdate() override {}
    void Update(float deltaTime) override;
    void Render(HDC hDC) override;

private:

    GameObjectBase* m_pBackground = nullptr;
    //GameObjectBase* m_pStartButton = nullptr;

    MyFirstWndGame* m_pGame = nullptr;

    WCHAR m_szTitle[64] = L"Title Scene";

    RECT m_rect = { 0, 0, 0, 0 };
};

