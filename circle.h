#ifndef GUARD_CIRCLE_H
#define GUARD_CIRCLE_H

struct circle2d {
	struct vector2d center;
	double radius;
};
enum circle2d_line2d_reln {
	TANGENT,
	INTERSECT,
	NONE
};
enum circle2d_line2d_reln check_intersect2d(const struct circle2d *c, const struct line2d_implicit *li);

#endif
