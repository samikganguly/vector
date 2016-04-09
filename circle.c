#include "vector.h"
#include "line.h"
#include "circle.h"

enum circle2d_line2d_reln check_intersect2d(const struct circle2d *c, const struct line2d_implicit *li) {	
	double m = li->normal.x * li->normal.x + li->normal.y * li->normal.y;
	m /= (li->param + dot2d(li->normal, c->center));
	struct vector2d dv = scalar_multiply2d(m, li->normal);
	m = dv.x * dv.x + dv.y * dv.y;
	m = m - c->radius * c->radius;
	if(m == 0) return TANGENT;
	if(m > 0) return NONE;
	return INTERSECT;
}
