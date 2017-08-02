#include "Maths.h"

rocks_in_space::vector_2d rocks_in_space::rotate(const vector_2d& point, float theta, const vector_2d& origin)
{
	const auto translation = point - origin;
	const auto s = sin(theta);
	const auto c = cos(theta);

	return vector_2d{ translation.x() * c - translation.y() * s, translation.x() * s + translation.y() * c } +origin;
}

bool rocks_in_space::intersects(vector_2d a1, vector_2d a2, vector_2d b1, vector_2d b2)
{
	assert(!(a1 == a2 || b1 == b2));

	// First test: do they share an end point?
	if ((a1 == b1) ||
		(a2 == b1) ||
		(a1 == b2) ||
		(a2 == b2))
		return false;

	// Translate the system so that point a1 is on the origin.
	a2 -= a1;
	b1 -= a1;
	b2 -= a1;

	// Rotate the system so that point a2 is on the positive x axis.
	auto a2_length = a2.magnitude();
	auto a2_cos = a2.x() / a2_length;
	auto a2_sin = a2.y() / a2_length;
	auto new_x = b1.x() * a2_cos + b1.y() * a2_sin;
	b1.y(b1.y() * a2_cos - b1.x() * a2_sin);
	b1.x(new_x);
	new_x = b2.x() * a2_cos + b2.y() * a2_sin;
	b2.y(b2.y() * a2_cos - b2.x() * a2_sin);
	b2.x(new_x);

	// Second test: do the endpoints of b both lie above or both lie below the x axis?
	if ((b1.y() < 0.0 && b2.y() < 0.0) ||
		(b1.y() >= 0.0 && b2.y() >= 0.0))
		return false;

	// Discover the position of the intersection point along line a.
	auto intersection_point = b2.x() + (b1.x() - b2.x()) * b2.y() / (b2.y() - b1.y());

	// Final test: does segment b crosses line a outside of segment a?
	return !(intersection_point < 0.0 || intersection_point > a2_length);
}

bool rocks_in_space::intersects(const rectangle& a, const rectangle& b)
{
	return contains(a, b.top_left()) ||
		contains(a, b.bottom_right()) ||
		contains(a, vector_2d(b.top(), b.right())) ||
		contains(a, vector_2d(b.bottom(), b.left())) ||
		contains(b, a.top_left()) ||
		contains(b, a.bottom_right()) ||
		contains(b, vector_2d(a.top(), a.right())) ||
		contains(b, vector_2d(a.bottom(), a.left()));
}

bool rocks_in_space::contains(const rectangle& r, const vector_2d& p)
{
	return p.x() >= r.left() &&
		p.x() <= r.right() &&
		p.y() >= r.top() &&
		p.y() <= r.bottom();
}

float rocks_in_space::radius(const rectangle& r)
{
	return (r.top_left() - r.bottom_right()).magnitude() / 2.0f;
}

rocks_in_space::rectangle rocks_in_space::translate(const rectangle& r, const vector_2d& v)
{
	return{ r.x() + v.x(), r.y() + v.y(), r.width(), r.height() };
}