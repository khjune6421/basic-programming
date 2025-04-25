#pragma once
#include "Collider.h"
#include "GameWindow.h"
#include <assert.h>
#include "Render.h"

class BitmapInfo;

enum ObjectType
{
	PLAYER,
	ENEMY
};

class GameObject
{
public:
	~GameObject()
	{
		delete m_pCollider;
		DeleteObject(m_hPenArr[0]);
		DeleteObject(m_hPenArr[1]);
	}
	GameObject(ObjectType type) : m_type(type) {}
	ObjectType Type() const { return m_type; }
	void SetPosition(float x, float y) { m_pos = { x, y }; }
	void SetPositionX(float x) { m_pos.x = x; }
	void SetPositionY(float y) { m_pos.y = y; }
	Vector GetPosition() const { return m_pos; }
	void SetCollider(float halfWidth, float halfHeight);
	void SetColliderW(float halfWidth);
	void SetColliderH(float halfHeight);
	Collider* GetCollider() const { return m_pCollider; }
	void Setm_pos(Vector newVecter) { m_pos.operator+=(newVecter); }
	void DrawCollider(HDC hdc, bool isColliding);
	void SetWidth(int width) { m_width = width; }
	void SetHeight(int height) { m_height = height; }

protected:

	ObjectType m_type;
	Vector m_pos = { 0.0f, 0.0f };
	Collider* m_pCollider = nullptr;
	HPEN m_hPenArr[2] = { CreatePen(PS_SOLID, 2, RGB(255, 0, 0)), CreatePen(PS_SOLID, 2, RGB(0, 0, 255)) };

	int m_width = 0;
	int m_height = 0;
	BitmapInfo* m_pBitmapInfo = nullptr;
};

class Player : public GameObject
{
public:
	Player(ObjectType type) : GameObject(type) {};

	void UpdateFrame(double deltatime);
	void SetBitmapInfo(BitmapInfo* bitmapInfo);
	void Update(bool isDucking);
	void Render(HDC hdc, bool isColliding);
	void Move(double deltaTime, float movement) { m_pos.x += movement * static_cast <float>(deltaTime); };

protected:
	void DrawBitmap(HDC hdc);

	struct FrameFPos
	{
		int x;
		int y;
	};

	FrameFPos m_frameXY[14] = { { 0, 0 }, };

	int m_frameWidth = 0;
	int m_frameHeight = 0;
	int m_frameIndex = 0;
	int m_frameAmount = 2;

	double m_frameTime = 0.0;
	double m_frameDuration = 100.0;
};

class Enemy : public GameObject
{
public:
	Enemy(ObjectType type) : GameObject(type) {};

	void UpdateFrame(double deltatime);
	void SetSpeed(float speed) { m_speed = speed; }
	void Move(double deltaTime) {m_pos.x += m_speed * static_cast <float>(deltaTime);}
	void SetBitmapInfo(BitmapInfo* bitmapInfo);
	bool Update(double deltaTime);
	void Render(HDC hdc, bool isColliding);

protected:
	void DrawBitmap(HDC hdc);

	struct FrameFPos
	{
		int x;
		int y;
	};

	FrameFPos m_frameXY[14] = { { 0, 0 }, };

	int m_frameWidth = 0;
	int m_frameHeight = 0;
	int m_frameIndex = 0;
	int m_frameAmount = 2;

	double m_frameTime = 0.0;
	double m_frameDuration = 200.0;

private:
	float m_speed = 0.0f;
	double lifeSpan = 10000.0;
};