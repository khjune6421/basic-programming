#pragma once
#include "Scene.h"

class GameObjectBase;
class GameObject;
class MyFirstWndGame;

class PlayScene :public Scene
{
 public:
    PlayScene() = default;
    ~PlayScene() override = default;

    void Initialize(NzWndBase* pWnd) override;
    void Finalize() override;

    virtual void Enter() override;
    virtual void Leave() override;

    void FixedUpdate() override;
    void Update(float deltaTime) override;
    void Render(HDC hDC) override;
  
private:
    void CreatePlayer();
    void CreateEnemy();

    void UpdatePlayerInfo();
    void UpdateEnemyInfo();

    GameObject* GetPlayer() const { return (GameObject*)m_GameObjectPtrTable[0]; }

    MyFirstWndGame* m_pGame = nullptr;
};

