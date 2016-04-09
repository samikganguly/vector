#include <math.h>
#include "vector.h"

double magnitude2d(struct vector2d v) {
	return sqrt(10000 * (v.x * v.x + v.y * v.y)) / 100;
}

struct vector2d normalize2d(struct vector2d v) {
	double m = magnitude2d(v);
	return (struct vector2d){v.x / m, v.y / m};
}

struct vector2d normal2d(struct vector2d v) {
	return (struct vector2d){-(v.y), v.x};
}

struct vector2d reverse2d(struct vector2d v) {
	return (struct vector2d){-(v.x), -(v.y)};
}

struct vector2d add2d(struct vector2d v1, struct vector2d v2) {
	return (struct vector2d){v1.x + v2.x, v1.y + v2.y};
}

struct vector2d subtract2d(struct vector2d v1, struct vector2d v2) {
	return (struct vector2d){v1.x - v2.x, v1.y - v2.y};
}

double distance2d(struct vector2d v1, struct vector2d v2) {
	return magnitude2d(subtract2d(v1, v2));
}

struct vector2d scalar_multiply2d(double k, struct vector2d v) {
	return (struct vector2d){k * v.x, k * v.y};
}

double dot2d(struct vector2d v1, struct vector2d v2) {
	return v1.x * v2.x + v1.y * v2.y;
}

struct vector2d rotate2d(double radian, struct vector2d v) {
	return (struct vector2d){
		v.x * cos(radian) - v.y * sin(radian),
		v.x * sin(radian) + v.y * cos(radian)
	};
}
