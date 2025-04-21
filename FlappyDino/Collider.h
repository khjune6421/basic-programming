#pragma once
#include "Vector2D.h"

struct Collider
{
	Vector center;
	Vector halfSize;
};

bool Intersect(Collider const& lhs, Collider const& rhs);