#pragma once
#include "INC_Windows.h"
#include <iostream>
#include <math.h>

namespace simplegeo
{
    class ShapeBase
    {
    public:
        virtual void Draw(HDC hdc) = 0;
        virtual ~ShapeBase() {}
    };

    class Circle : public ShapeBase
    {
    public:
        Circle(int centerX, int centerY, int radius, COLORREF color)
            : m_centerX(centerX), m_centerY(centerY), m_radius(radius), m_color(color) {
        }

        void Draw(HDC hdc) override
        {
            // 펜 생성 및 선택
            HPEN hPen = CreatePen(PS_SOLID, 2, m_color);
            HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
            // 브러시는 내부 채우지 않도록 NULL_BRUSH 사용
            HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));

            Ellipse(hdc,
                m_centerX - m_radius, m_centerY - m_radius,
                m_centerX + m_radius, m_centerY + m_radius);

            // 이전 객체 복원 및 펜 삭제
            SelectObject(hdc, hOldPen);
            SelectObject(hdc, hOldBrush);
            DeleteObject(hPen);
        }
    private:
        int m_centerX, m_centerY, m_radius;
        COLORREF m_color;
    };


    class RectangleShape : public ShapeBase
    {
    public:
        RectangleShape(int left, int top, int right, int bottom, COLORREF color)
            : m_left(left), m_top(top), m_right(right), m_bottom(bottom), m_color(color) {
        }

        void Draw(HDC hdc) override
        {
            HPEN hPen = CreatePen(PS_SOLID, 2, m_color);
            HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
            HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));

            Rectangle(hdc, m_left, m_top, m_right, m_bottom);

            SelectObject(hdc, hOldPen);
            SelectObject(hdc, hOldBrush);
            DeleteObject(hPen);
        }

    private:
        int m_left, m_top, m_right, m_bottom;
        COLORREF m_color;
    };


    class Line : public ShapeBase
    {
    public:
        Line(int startX, int startY, int endX, int endY, COLORREF color)
            : m_startX(startX), m_startY(startY), m_endX(endX), m_endY(endY), m_color(color) {
        }

        void Draw(HDC hdc) override
        {
            HPEN hPen = CreatePen(PS_SOLID, 2, m_color);
            HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);

            MoveToEx(hdc, m_startX, m_startY, NULL);
            LineTo(hdc, m_endX, m_endY);

            SelectObject(hdc, hOldPen);
            DeleteObject(hPen);
        }
    private:
        int m_startX, m_startY, m_endX, m_endY;
        COLORREF m_color;
    };

    constexpr int MAX_SHAPES = 100; // 최대 도형 개수
    class GeoShapeManager
    {
    public:
        GeoShapeManager() = default;
        ~GeoShapeManager()
        {
            for (int i = 0; i < MAX_SHAPES; ++i)
            {
                if (m_shapes[i] != nullptr)
                {
                    delete m_shapes[i];
                    m_shapes[i] = nullptr;
                }
            }
        }

        void AddCircle(int centerX, int centerY, int radius, COLORREF color)
        {
            if (m_shapeCount >= MAX_SHAPES) return;

            for (int i = 0; i < m_shapeCount; i++)
            {
                if (circleArray[i][0])
                {
                    if (pow(circleArray[i][1] - centerX, 2) + pow(circleArray[i][2] - centerY, 2) <= pow(radius, 2))
                    {
                        m_shapes[i] = nullptr;
                        circleArray[i][0] = 0;
                        ++m_shapeCount;
                        return;
                    }
                }
                else if (rackArray[i][0])
                {
                    if (rackArray[i][1] <= centerX && rackArray[i][1] + 20 >= centerX && rackArray[i][2] <= centerY && rackArray[i][2] + 20 >= centerY)
                    {
                        m_shapes[i] = nullptr;
                        rackArray[i][0] = 0;
                        ++m_shapeCount;
                        return;
                    }
                }
            }
            m_shapes[m_shapeCount] = new Circle(centerX, centerY, radius, color);
            circleArray[m_shapeCount][0] = 1;
            circleArray[m_shapeCount][1] = centerX;
            circleArray[m_shapeCount][2] = centerY;
            ++m_shapeCount;
        }

        void AddRectangle(int left, int top, int right, int bottom, COLORREF color)
        {
            if (m_shapeCount >= MAX_SHAPES) return;

            for (int i = 0; i < m_shapeCount; i++)
            {
                if (circleArray[i][0])
                {
                    if (pow(circleArray[i][1] - (left + 10), 2) + pow(circleArray[i][2] - (top + 10), 2) <= pow(10, 2))
                    {
                        m_shapes[i] = nullptr;
                        circleArray[i][0] = 0;
                        ++m_shapeCount;
                        return;
                    }
                }
                else if (rackArray[i][0])
                {
                    if (rackArray[i][1] <= left + 10 && rackArray[i][1] + 20 >= left + 10 && rackArray[i][2] <= top + 10 && rackArray[i][2] + 20 >= top + 10)
                    {
                        m_shapes[i] = nullptr;
                        rackArray[i][0] = 0;
                        ++m_shapeCount;
                        return;
                    }
                }
            }
            m_shapes[m_shapeCount] = new RectangleShape(left, top, right, bottom, color);
            rackArray[m_shapeCount][0] = 1;
            rackArray[m_shapeCount][1] = left;
            rackArray[m_shapeCount][2] = top;
            ++m_shapeCount;
        }

        void AddLine(int startX, int startY, int endX, int endY, COLORREF color)
        {
            if (m_shapeCount >= MAX_SHAPES) return;

            m_shapes[m_shapeCount] = new Line(startX, startY, endX, endY, color);
            ++m_shapeCount;
        }

        void Draw(HDC hdc)
        {
            for (int i = 0; i < MAX_SHAPES; ++i)
            {
                if (m_shapes[i] == nullptr) continue;

                m_shapes[i]->Draw(hdc); // 컴파일 타임에는 객테가 어떤 타입인지 알 수 없음.
            }
        }
    private:
        int circleArray[MAX_SHAPES][3] = { 0, };
        int rackArray[MAX_SHAPES][3] = { 0, };

    private:
        ShapeBase* m_shapes[MAX_SHAPES] = { nullptr, };
        int m_shapeCount = 0;
    };

    extern GeoShapeManager g_GeoShapeManager;
}