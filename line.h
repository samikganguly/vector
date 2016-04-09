#ifndef GUARD_LINE_H
#define GUARD_LINE_H
struct vector2d;

struct line2d_implicit {
	struct vector2d normal;
	double param;
};
struct line2d_explicit {
	struct vector2d point;
	struct vector2d dir;
	double param;
};
struct ray2d_implicit {
	struct line2d_implicit line;
	struct vector2d origin;
};
struct ray2d_explicit {
	struct line2d_explicit line;
	struct vector2d origin;
};
void to_implicit2d(const struct line2d_explicit *le, struct line2d_implicit *li);
void to_explicit2d(const struct line2d_implicit *li, struct line2d_explicit *le);
struct vector2d find_intersect2d(const struct line2d_explicit *le, 
	const struct line2d_implicit *li, unsigned char *is_parallel);
struct vector2d point_to_line_nearest2d(const struct vector2d *point,
	const struct line2d_implicit *li); 
struct vector2d point_to_ray_nearest2d(const struct vector2d *point,
	const struct ray2d_implicit *ri);
unsigned char point_on_line2d(const struct vector2d *point, const struct line2d_explicit *le);
unsigned char point_on_ray2d(const struct vector2d *point, const struct ray2d_explicit *re);
#endif
