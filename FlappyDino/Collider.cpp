#include "Collider.h"

bool Intersect(Collider const& lhs, Collider const& rhs)
{
	if (lhs.center.x - lhs.halfSize.x > rhs.center.x + rhs.halfSize.x) return false;
	if (lhs.center.x + lhs.halfSize.x < rhs.center.x - rhs.halfSize.x) return false;

	if (lhs.center.y - lhs.halfSize.y > rhs.center.y + rhs.halfSize.y) return false;
	if (lhs.center.y + lhs.halfSize.y < rhs.center.y - rhs.halfSize.y) return false;

	return true;
}