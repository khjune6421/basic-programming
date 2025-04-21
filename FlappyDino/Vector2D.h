#pragma once
#include <cmath>

struct Vector
{
	float x, y;

	Vector() : x(0), y(0) {}
	Vector(float value) : x(value), y(value) {}
	Vector(float a, float b) : x(a), y(b) {}
	Vector(const Vector& other) : x(other.x), y(other.y) {}

	Vector& operator=(const Vector& other)
	{
		if (this != &other)
		{
			x = other.x;
			y = other.y;
		}
		return *this;
	}

	Vector operator+(const Vector& other) const
	{
		return Vector(x + other.x, y + other.y);
	}

	Vector operator-(const Vector& other) const
	{
		return Vector(x - other.x, y - other.y);
	}

	Vector operator*(float scalar) const
	{
		return Vector(x * scalar, y * scalar);
	}

	Vector operator/(float scalar) const
	{
		if (scalar != 0)
			return Vector(x / scalar, y / scalar);
		else
			return Vector(0, 0); // Avoid division by zero
	}

	Vector& operator+=(const Vector& other)
	{
		x += other.x;
		y += other.y;
		return *this;
	}

	Vector& operator-=(const Vector& other)
	{
		x -= other.x;
		y -= other.y;
		return *this;
	}

	Vector& operator*=(float scalar)
	{
		x *= scalar;
		y *= scalar;
		return *this;
	}

	Vector& operator/=(float scalar)
	{
		if (scalar != 0)
		{
			x /= scalar;
			y /= scalar;
		}
		return *this;
	}

	bool operator==(const Vector& other) const
	{
		return (x == other.x && y == other.y);
	}

	bool operator!=(const Vector& other) const
	{
		return !(*this == other);
	}

	Vector operator-() const
	{
		return Vector(-x, -y);
	}

	float Length() const
	{
		return sqrt(x * x + y * y);
	}

	float LengthSquared() const
	{
		return x * x + y * y;
	}

	float Normalize()
	{
		float length = Length();
		if (length > 0)
		{
			x /= length;
			y /= length;
		}
		return length;
	}

	float Dot(const Vector& other) const
	{
		return x * other.x + y * other.y;
	}

	float Cross(const Vector& other) const
	{
		return x * other.y - y * other.x;
	}

	float Distance(const Vector& other) const
	{
		return sqrt((x - other.x) * (x - other.x) + (y - other.y) * (y - other.y));
	}

	float DistanceSquared(const Vector& other) const
	{
		return (x - other.x) * (x - other.x) + (y - other.y) * (y - other.y);
	}
};

struct Utillity
{
	int g_width = 800;
	int g_height = 600;

	void SetScreenSize(int width, int height)
	{
		g_width = width;
		g_height = height;
	}

	void GetScreenSize(int& width, int& height)
	{
		width = g_width;
		height = g_height;
	}
};