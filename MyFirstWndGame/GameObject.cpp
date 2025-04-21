#include "INC_Windows.h"
#include "Collider.h"
#include "GameObject.h"
#include "RenderHelp.h"
#include <assert.h>
#include "NzWndBase.h"

GameObject::~GameObject()
{
    if (m_pColliderCircle)
    {
        delete m_pColliderCircle;
        m_pColliderCircle = nullptr;
    }

    if (m_pColliderBox)
    {
        delete m_pColliderBox;
        m_pColliderBox = nullptr;
    }
    DeleteObject(hPenArr[0]);
    DeleteObject(hPenArr[1]);
}

void GameObject::Update(float deltaTime)
{
    Move(deltaTime);

    // Collider ������Ʈ
    if (m_pColliderCircle)
    {
        m_pColliderCircle->center = m_pos;
    }
    if (m_pColliderBox)
    {
        m_pColliderBox->center = m_pos;
    }
}

void GameObject::Render(HDC hdc, bool isColliding)
{
    DrawCollider(hdc, isColliding);
	DrawBitmap(hdc);
}


void GameObject::SetColliderCircle(float radius)
{
    if (m_pColliderCircle)
    {
        delete m_pColliderCircle;
        m_pColliderCircle = nullptr;
    }

    m_pColliderCircle = new ColliderCircle;

    assert(m_pColliderCircle != nullptr && "Failed to create ColliderCircle!");

    m_pColliderCircle->radius = radius;
    m_pColliderCircle->center = m_pos;
}


void GameObject::SetColliderBox(float width, float height)
{
    if (m_pColliderBox)
    {
        delete m_pColliderBox;
        m_pColliderBox = nullptr;
    }

    m_pColliderBox = new ColliderBox;

    assert(m_pColliderBox != nullptr && "Failed to create ColliderBox!");

    m_pColliderBox->center = m_pos;
    m_pColliderBox->halfSize.x = width / 2.0f;
    m_pColliderBox->halfSize.y = height / 2.0f;
}

void GameObject::SetBitmapInfo(BitmapInfo* bitmapInfo)
{
    assert(m_pBitmapInfo == nullptr && "BitmapInfo must be null!");

    m_pBitmapInfo = bitmapInfo;


    // ��������Ʈ ������ �ϴ��� �ϵ��ڵ��ؿ�. 
    // �ϴ�, ������ ũ��� �ð��� ���ٰ� �����մϴ�.
    m_frameWidth = m_pBitmapInfo->GetWidth() / 5;
    m_frameHeight = m_pBitmapInfo->GetHeight() / 3;
    m_frameIndex = 0;

    for (int i = 0; i < 5; ++i)
    {
        m_frameXY[i].x = i * m_frameWidth;
        m_frameXY[i].y = 0;
    }

    for (int i = 0; i < 5; ++i)
    {
        m_frameXY[i + 5].x = i * m_frameWidth;
        m_frameXY[i + 5].y = m_frameHeight;
    }

    for (int i = 0; i < 4; ++i)
    {
        m_frameXY[i + 10].x = i * m_frameWidth;
        m_frameXY[i + 10].y = m_frameHeight * 2;
    }

}

// ����: �ش� �ڵ��� ������ �����ϱ��? ��� �����ϸ� �������?
// ���� ���⿡ ���� ���� ����ϰ� ���� ������.
void GameObject::DrawCollider(HDC hdc, bool isColliding)
{
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPenArr[isColliding]);
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));

    if (m_pColliderCircle)
    {
        Ellipse(hdc, m_pColliderCircle->center.x - m_pColliderCircle->radius,
            m_pColliderCircle->center.y - m_pColliderCircle->radius,
            m_pColliderCircle->center.x + m_pColliderCircle->radius,
            m_pColliderCircle->center.y + m_pColliderCircle->radius);
    }

    else if (m_pColliderBox)
    {
        Rectangle(hdc, m_pColliderBox->center.x - m_pColliderBox->halfSize.x,
            m_pColliderBox->center.y - m_pColliderBox->halfSize.y,
            m_pColliderBox->center.x + m_pColliderBox->halfSize.x,
            m_pColliderBox->center.y + m_pColliderBox->halfSize.y);
    }

    // ���� ��ü ���� �� �� ����
    SelectObject(hdc, hOldPen);
    SelectObject(hdc, hOldBrush);
}

void GameObject::Move(float deltaTime)
{
    GameObjectBase::Move(deltaTime);
}

void GameObjectBase::SetName(const char* name)
{
    if (name == nullptr) return;

    strncpy_s(m_name, name, OBJECT_NAME_LEN_MAX - 1);
    m_name[OBJECT_NAME_LEN_MAX - 1] = '\0';
}

void GameObject::DrawBitmap(HDC hdc)
{
    if (m_pBitmapInfo == nullptr) return;
    if (m_pBitmapInfo->GetBitmapHandle() == nullptr) return;

	HDC hBitmapDC = CreateCompatibleDC(hdc); // �޸𸮿� ȣȯ�Ǵ� DC ����

    HBITMAP hOldBitmap = (HBITMAP)SelectObject(hBitmapDC, m_pBitmapInfo->GetBitmapHandle());
    // BLENDFUNCTION ���� (���� ä�� ó��)
    BLENDFUNCTION blend = { 0 };
    blend.BlendOp = AC_SRC_OVER;
    blend.SourceConstantAlpha = 255;  // ���� ���� ä�� �״�� ���
    blend.AlphaFormat = AC_SRC_ALPHA;

    const int x = m_pos.x - m_width / 2;
    const int y = m_pos.y - m_height / 2;

    const int srcX = m_frameXY[m_frameIndex].x;
    const int srcY = m_frameXY[m_frameIndex].y;

    AlphaBlend(hdc, x, y, m_width, m_height,
        hBitmapDC, srcX, srcY, m_frameWidth, m_frameHeight, blend);

    // ��Ʈ�� �ڵ� ����
    SelectObject(hBitmapDC, hOldBitmap);
    DeleteDC(hBitmapDC);

}

void GameObject::UpdateFrame(float deltaTime)
{
    m_frameTime += deltaTime;
    if (m_frameTime >= m_frameDuration)
    {
        m_frameTime = 0.0f;
        m_frameIndex = (m_frameIndex + 1) % (m_frameCount);
    }
}
