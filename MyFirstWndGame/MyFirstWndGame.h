#pragma once
#include "NzWndBase.h"
#include "Utillity.h"
#include "Scene.h"
#include "GameObject.h"

class Scene;
namespace renderHelp
{
    class BitmapInfo;
}
// [CHECK] #7  ���� ������ ����Ͽ� ��������� �������� ����.
class GameTimer;
class GameObjectBase;
class GameObject;

class MyFirstWndGame : public NzWndBase
{
public:
    MyFirstWndGame() = default;
    ~MyFirstWndGame() override = default;

    bool Initialize();
    void Run();
    void Finalize();

private:
    void Update();
    void Render();

    void OnResize(int width, int height) override;
    void OnClose() override;

    void OnMouseMove(int x, int y);
    void OnLButtonDown(int x, int y);
    void OnRButtonDown(int x, int y);

    void FixedUpdate();
    void LogicUpdate();

    void CreatePlayer();
    void CreateEnemy();

    void UpdatePlayerInfo();
	

    GameObject* GetPlayer() const { return (GameObject*)m_GameObjectPtrTable[0]; }

private:
    HDC m_hFrontDC = nullptr;
    HDC m_hBackDC = nullptr;
    HBITMAP m_hBackBitmap = nullptr;
    HBITMAP m_hDefaultBitmap = nullptr;

    // [CHECK] #8. ���� Ÿ�̸Ӹ� ����Ͽ� �������� �����ϴ� ����.
    GameTimer* m_pGameTimer = nullptr;
    float m_fDeltaTime = 0.0f;
    float m_fFrameCount = 0.0f;

    int m_eCurrentScene = SCENE_TITLE;
    Scene* m_pScenes[SceneType::SCENE_MAX] = { nullptr, nullptr, nullptr };
    // [CHECK] #8. ���� ������Ʈ�� �����ϴ� �����̳�.
    GameObjectBase** m_GameObjectPtrTable = nullptr;

    struct MOUSE_POS
    {
        int x = 0;
        int y = 0;

        bool operator!=(const MOUSE_POS& other) const
        {
            return (x != other.x || y != other.y);
        }
    };

    MOUSE_POS m_MousePos = { 0, 0 };
    MOUSE_POS m_MousePosPrev = { 0, 0 };

    MOUSE_POS m_PlayerTargetPos = { 0, 0 };
    MOUSE_POS m_EnemySpawnPos = { 0, 0 };

#pragma region resource
    using BitmapInfo = renderHelp::BitmapInfo;

    BitmapInfo* m_pPlayerBitmapInfo = nullptr;

    BitmapInfo* m_pEnemyBitmapInfo = nullptr;

    BitmapInfo* m_pBackgroundBitmapInfo = nullptr;
    
public:
    using Vector2f = learning::Vector2f;
    Vector2f EnemySpawnPosition();
    Vector2f PlayerTargetPosition();
    void ResetEnemySpawnPosition();
    BitmapInfo* GetPlayerBitmapInfo();
    BitmapInfo* GetEnemyBitmapInfo();
    BitmapInfo* GetBackgroundBitmapInfo() const { return m_pBackgroundBitmapInfo; }
    void ChangeScene(SceneType eSceneType);
#pragma endregion
};