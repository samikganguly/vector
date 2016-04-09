#include <math.h>
#include "vector.h"
#include "line.h"

void to_implicit2d(const struct line2d_explicit *le, struct line2d_implicit *li) {
	li->normal = normal2d(le->dir);
	li->param = - dot2d(li->normal, le->point);
}

void to_explicit2d(const struct line2d_implicit *li, struct line2d_explicit *le) {
	le->point.x = 0;
	le->point.y = -li->param / li->normal.y;
	le->dir = scalar_multiply2d(-1, normal2d(li->normal));
}

struct vector2d point_to_line_nearest2d(const struct vector2d *point,
	const struct line2d_implicit *li) {
	const double m = li->normal.x * li->normal.x + li->normal.y * li->normal.y;
	const double c = (li->param + dot2d(li->normal, *point)) / m;
	return subtract2d(*point, scalar_multiply2d(c, li->normal));
}

struct vector2d find_intersect2d(const struct line2d_explicit *le, 
	const struct line2d_implicit *li, unsigned char *is_parallel) {
	struct vector2d poi = {0, 0};
	const double d = dot2d(le->dir, li->normal);
	if(d == 0)
		*is_parallel = 1;
	else {
		*is_parallel = 0;
		const double c = (dot2d(li->normal, le->point) + li->param) / d;
		poi = subtract2d(le->point, scalar_multiply2d(c, le->dir));
	}
	return poi;
}
