#include "GameObject.h"

void Player::Render(HDC hdc, bool isColliding)
{
	DrawCollider(hdc, isColliding);
    Player::DrawBitmap(hdc);
}

void Enemy::Render(HDC hdc, bool isColliding)
{
    DrawCollider(hdc, isColliding);
    Enemy::DrawBitmap(hdc);
}

void GameObject::SetCollider(float width, float height)
{
    if (m_pCollider)
    {
        delete m_pCollider;
        m_pCollider = nullptr;
    }

    m_pCollider = new Collider;

    assert(m_pCollider != nullptr && "콜라이더 생성 실패");

    m_pCollider->center = m_pos;
    m_pCollider->halfSize.x = width / 2.0f;
    m_pCollider->halfSize.y = height / 2.0f;
}

void GameObject::SetColliderW(float width)
{
    float height = 0.0f;
    if (m_pCollider)
    {
        height = m_pCollider->halfSize.y;
        delete m_pCollider;
        m_pCollider = nullptr;
    }

    m_pCollider = new Collider;

    assert(m_pCollider != nullptr && "콜라이더 생성 실패");

    m_pCollider->center = m_pos;
    m_pCollider->halfSize.x = width / 2.0f;
    if (height)
    {
        m_pCollider->halfSize.y = height;
    }
}

void GameObject::SetColliderH(float height)
{
    float width = 0.0f;
    if (m_pCollider)
    {
        width = m_pCollider->halfSize.x;
        delete m_pCollider;
        m_pCollider = nullptr;
    }

    m_pCollider = new Collider;

    assert(m_pCollider != nullptr && "콜라이더 생성 실패");

    m_pCollider->center = m_pos;
    if (width)
    {
        m_pCollider->halfSize.x = width;
    }
    m_pCollider->halfSize.y = height / 2.0f;
}

void GameObject::DrawCollider(HDC hdc, bool isColliding)
{
    HPEN hOldPen = (HPEN)SelectObject(hdc, m_hPenArr[isColliding]);
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));

    Rectangle(hdc, m_pCollider->center.x - m_pCollider->halfSize.x, m_pCollider->center.y - m_pCollider->halfSize.y, m_pCollider->center.x + m_pCollider->halfSize.x, m_pCollider->center.y + m_pCollider->halfSize.y);

    SelectObject(hdc, hOldPen);
    SelectObject(hdc, hOldBrush);
}

void Player::DrawBitmap(HDC hdc)
{
    if (m_pBitmapInfo == nullptr) return;
    if (m_pBitmapInfo->GetBitmapHandle() == nullptr) return;

    HDC hBitmapDC = CreateCompatibleDC(hdc);

    HBITMAP hOldBitmap = (HBITMAP)SelectObject(hBitmapDC, m_pBitmapInfo->GetBitmapHandle());
    BLENDFUNCTION blend = { 0 };
    blend.BlendOp = AC_SRC_OVER;
    blend.SourceConstantAlpha = 255;
    blend.AlphaFormat = AC_SRC_ALPHA;

    const int x = m_pos.x - m_width / 2;
    const int y = m_pos.y - m_height / 2;

    const int srcX = m_frameXY[m_frameIndex].x;
    const int srcY = m_frameXY[m_frameIndex].y;

    AlphaBlend(hdc, x, y, m_width, m_height,
        hBitmapDC, srcX, srcY, m_frameWidth, m_frameHeight, blend);

    SelectObject(hBitmapDC, hOldBitmap);
    DeleteDC(hBitmapDC);
}

void Enemy::DrawBitmap(HDC hdc)
{
    if (m_pBitmapInfo == nullptr) return;
    if (m_pBitmapInfo->GetBitmapHandle() == nullptr) return;

    HDC hBitmapDC = CreateCompatibleDC(hdc);

    HBITMAP hOldBitmap = (HBITMAP)SelectObject(hBitmapDC, m_pBitmapInfo->GetBitmapHandle());
    BLENDFUNCTION blend = { 0 };
    blend.BlendOp = AC_SRC_OVER;
    blend.SourceConstantAlpha = 255;
    blend.AlphaFormat = AC_SRC_ALPHA;

    const int x = m_pos.x - m_width / 2;
    const int y = m_pos.y - m_height / 2;

    const int srcX = m_frameXY[m_frameIndex].x;
    const int srcY = m_frameXY[m_frameIndex].y;

    AlphaBlend(hdc, x, y, m_width, m_height,
        hBitmapDC, srcX, srcY, m_frameWidth, m_frameHeight, blend);

    SelectObject(hBitmapDC, hOldBitmap);
    DeleteDC(hBitmapDC);
}

void Player::UpdateFrame(double deltaTime)
{
    m_frameTime += deltaTime;
    if (m_frameTime >= m_frameDuration)
    {
        m_frameTime = 0.0;
        m_frameIndex = (m_frameIndex + 1) % (m_frameAmount);
    }
}

void Enemy::UpdateFrame(double deltaTime)
{
    m_frameTime += deltaTime;
    if (m_frameTime >= m_frameDuration)
    {
        m_frameTime = 0.0;
        m_frameIndex = (m_frameIndex + 1) % (m_frameAmount);
    }
}

void Player::SetBitmapInfo(BitmapInfo* bitmapInfo)
{
    m_pBitmapInfo = bitmapInfo;
    m_frameWidth = m_pBitmapInfo->GetWidth() / 2;
    m_frameHeight = m_pBitmapInfo->GetHeight();

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

void Enemy::SetBitmapInfo(BitmapInfo* bitmapInfo)
{
    assert(m_pBitmapInfo == nullptr && "BitmapInfo must be null!");

    m_pBitmapInfo = bitmapInfo;
    m_frameWidth = m_pBitmapInfo->GetWidth() / 2;
    m_frameHeight = m_pBitmapInfo->GetHeight();
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

void Player::Update(bool isDucking)
{
    Vector addVector = { 0.0f, 25.0f * isDucking };
    if (m_pCollider)
    {
        m_pCollider->center = m_pos.operator+(addVector);
    }
}

bool Enemy::Update(double deltaTime)
{
    Move(deltaTime);
    lifeSpan -= deltaTime;
    if (lifeSpan < 0)
    {
        return true;
    }
    else
    {
        if (m_pCollider)
        {
            m_pCollider->center = m_pos;
        }
        return false;
    }
}