#pragma once

#include "Utillity.h"
#include <algorithm>
// [CHECK]. namespace �����ؼ� ���� �����ؾ� ��
namespace learning
{
    struct ColliderCircle;
    struct ColliderBox;
}
namespace renderHelp
{
    class BitmapInfo;
}

enum class ObjectType
{
    PLAYER,
    ENEMY,
    BULLET,
    ITEM,
    BACKGROUND,
};

constexpr int OBJECT_NAME_LEN_MAX = 15;

class GameObjectBase
{
    using Vector2f = learning::Vector2f;
public:
    GameObjectBase() = delete;
    GameObjectBase(const GameObjectBase&) = delete;

    GameObjectBase(ObjectType type) : m_type(type) {}

    virtual ~GameObjectBase() = default;

    virtual void Update(float deltaTime) = 0;
    virtual void Render(HDC hdc, bool isColliding) = 0;

    void SetPosition(float x, float y) { m_pos = { x, y }; }
    void SetDirection(Vector2f dir) { m_dir = dir; }
    void SetSpeed(float speed) { m_speed = speed; }
    void SetName(const char* name);

    ObjectType Type() const { return m_type; }

    const char* GetName() const { return m_name; }

    Vector2f GetPosition() const { return m_pos; }
    Vector2f GetDirection() const { return m_dir; }

    float GetSpeed() const { return m_speed; }

protected:

    void Move(float deltaTime)
    {
        m_pos.x += m_dir.x * m_speed * deltaTime;
        m_pos.y += m_dir.y * m_speed * deltaTime;
    }

protected:
    ObjectType m_type;

    Vector2f m_pos = { 0.0f, 0.0f };
    Vector2f m_dir = { 0.0f, 0.0f }; // ���� (���� ����)

    float m_speed = 0.0f; // �ӷ�

    char m_name[OBJECT_NAME_LEN_MAX] = "";
};

class GameObject : public GameObjectBase
{
    using ColliderCircle = learning::ColliderCircle;
    using ColliderBox = learning::ColliderBox;
    using BitmapInfo = renderHelp::BitmapInfo;

public:
    GameObject(const GameObject&) = delete;
    GameObject(ObjectType type) : GameObjectBase(type) {}
    ~GameObject() override;

    void Update(float deltaTime) override;
    void Render(HDC hdc, bool isColliding) override;

    void SetColliderCircle(float radius);
    void SetColliderBox(float halfWidth, float halfHeight);

    void setWidth(int width) { m_width = width; }
    void setHeight(int height) { m_height = height; }

    ColliderCircle* GetColliderCircle() const
    {
        return m_pColliderCircle;
    }

    void SetBitmapInfo(BitmapInfo* bitmapInfo);
    void UpdateFrame(float deltaTime);

protected:
    void DrawCollider(HDC hdc, bool isColliding);

    void Move(float deltaTime);

    // Collider
    ColliderCircle* m_pColliderCircle = nullptr;
    ColliderBox* m_pColliderBox = nullptr;

	HPEN hPenArr[2] = { CreatePen(PS_SOLID, 2, RGB(255, 0, 0)), CreatePen(PS_SOLID, 2, RGB(0, 0, 255)) };

	int m_width = 0;
	int m_height = 0;

    void DrawBitmap(HDC hdc);

    // Bitmap ����
    BitmapInfo* m_pBitmapInfo = nullptr;

    // ���������� ���ڰ� ���� ���ƿ�.
    struct FrameFPos
    {
        int x;
        int y;
    };
    // ������ ����: �� 14���İ� �����ø� �ôٰ� �ۿ�...:)
    FrameFPos m_frameXY[14] = { { 0, 0 }, };
    int m_frameWidth = 0;
    int m_frameHeight = 0;
    int m_frameIndex = 0;
    int m_frameCount = 14; // ������ ��

    float m_frameTime = 0.0f;
    float m_frameDuration = 100.0f; // ���� ����
};