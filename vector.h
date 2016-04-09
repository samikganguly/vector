#ifndef GUARD_VECTOR_H
#define GUARD_VECTOR_H
struct vector2d {
	double x;
	double y;
};

double magnitude2d(struct vector2d v);
struct vector2d normalize2d(struct vector2d v);
struct vector2d normal2d(struct vector2d v);
#define perpendicular2d(x) normal2d(x)
struct vector2d reverse2d(struct vector2d v);
struct vector2d add2d(struct vector2d v1, struct vector2d v2);
struct vector2d subtract2d(struct vector2d v1, struct vector2d v2);
double distance2d(struct vector2d v1, struct vector2d v2);
struct vector2d scalar_multiply2d(double k, struct vector2d v);
double dot2d(struct vector2d v1, struct vector2d v2);
struct vector2d rotate2d(double radian, struct vector2d v);
#endif
