#ifndef GUARD_HELPER_H
#define GUARD_HELPER_H
#include <cairo/cairo.h>
#include <cairo/cairo-svg.h>
struct line2d_explicit;
struct line2d_implicit;
union line2d;
struct vector2d;
struct color {
	double r;
	double g;
	double b;
	double a;
};
struct graph {
	double origin[2];
	double dim[2];
	double scale[2];
	double grid_line_width;
	double axes_line_width;
	const struct color *grid_col;
	const struct color *axes_col;
};
struct cairo_resources {
	cairo_t *context;
	cairo_surface_t *surface;
};
int init_cairo_svg(const char *file, const struct graph *the_graph,
	struct cairo_resources *res);
void cleanup_cairo(struct cairo_resources *res);
void draw_grid_axes(cairo_t *context, const struct graph *the_graph);
void draw_point(cairo_t *context, const struct graph *the_graph,
	const struct vector2d *p, const struct color *col);
void draw_line_explicit(cairo_t *context, const struct graph *the_graph, 
	const struct line2d_explicit *l, const struct color *col,
	const double width);
void draw_line_implicit(cairo_t *context, const struct graph *the_graph, 
	const struct line2d_implicit *l, const struct color *col,
	const double width);
void draw_vector(cairo_t *context, const struct graph *the_graph,
	const struct vector2d *v, const struct color *col, const double width);
void draw_circle(cairo_t *context, const struct graph *the_graph,
	const struct circle2d *c, const struct color *stroke, const double width,
	const struct color *fill);
#endif
