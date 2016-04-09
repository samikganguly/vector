#include <cairo/cairo.h>
#include <cairo/cairo-svg.h>
#include <math.h>
#include "vector.h"
#include "line.h"
#include "circle.h"
#include "helper.h"

int init_cairo_svg(const char *file, const struct graph *the_graph,
	struct cairo_resources *res) {
	res->surface = cairo_svg_surface_create(file,
		the_graph->dim[0], the_graph->dim[1]);
	if(cairo_surface_status(res->surface) != CAIRO_STATUS_SUCCESS)
		return 0;
	res->context = cairo_create(res->surface);
	if(cairo_status(res->context) != CAIRO_STATUS_SUCCESS)
		return 0;
	return 1;
}

void cleanup_cairo(struct cairo_resources *res) {
	if(res->context)
		cairo_destroy(res->context);
	if(res->surface)
		cairo_surface_destroy(res->surface);
}

void draw_grid_axes(cairo_t *context, const struct graph *the_graph) {
	cairo_set_source_rgba(context, the_graph->axes_col->r,
		the_graph->axes_col->g, the_graph->axes_col->b,
		the_graph->axes_col->a);
	cairo_set_line_width(context, the_graph->axes_line_width);
	// X axis
	cairo_move_to(context, 0, the_graph->origin[1]);
	cairo_line_to(context, the_graph->dim[0], the_graph->origin[1]);
	
	cairo_new_sub_path(context);
	
	// Y axis
	cairo_move_to(context, the_graph->origin[0], 0);
	cairo_line_to(context, the_graph->origin[0], the_graph->dim[1]);
	cairo_stroke(context);
	
	cairo_set_source_rgba(context, the_graph->grid_col->r,
		the_graph->grid_col->g, the_graph->grid_col->b,
		the_graph->grid_col->a);
	cairo_set_line_width(context, the_graph->grid_line_width);
	double i;
	// X grids
	for(i = the_graph->origin[1] + the_graph->scale[1];
		i < the_graph->dim[1]; i += the_graph->scale[1]) {
		cairo_move_to(context, 0, i);
		cairo_line_to(context, the_graph->dim[0], i);
		cairo_new_sub_path(context);
	}
	for(i = the_graph->origin[1] + the_graph->scale[1];
		i > 0; i -= the_graph->scale[1]) {
		cairo_move_to(context, 0, i);
		cairo_line_to(context, the_graph->dim[0], i);
		cairo_new_sub_path(context);
	}
	// Y grids
	for(i = the_graph->origin[0] + the_graph->scale[0];
		i < the_graph->dim[0]; i += the_graph->scale[0]) {
		cairo_move_to(context, i, 0);
		cairo_line_to(context, i, the_graph->dim[1]);
		cairo_new_sub_path(context);
	}
	for(i = the_graph->origin[0] + the_graph->scale[0];
		i > 0; i -= the_graph->scale[0]) {
		cairo_move_to(context, i, 0);
		cairo_line_to(context, i, the_graph->dim[1]);
		cairo_new_sub_path(context);
	}
	cairo_stroke(context);
}

void draw_point(cairo_t *context, const struct graph *the_graph,
	const struct vector2d *p, const struct color *col) {
	cairo_set_source_rgba(context, col->r, col->g, col->b, col->a);
	cairo_set_line_width(context, 0);
	const struct vector2d ctr = {the_graph->origin[0] + the_graph->scale[0]
		* p->x, the_graph->origin[1] - the_graph->scale[1] * p->y};
	cairo_arc(context, ctr.x, ctr.y, 2, 0, M_PI * 2);
	cairo_fill(context);
}

void draw_line_explicit(cairo_t *context, const struct graph *the_graph, 
	const struct line2d_explicit *l, const struct color *col,
	const double width) {
	cairo_set_source_rgba(context, col->r, col->g, col->b, col->a);
	cairo_set_line_width(context, width);
	double intercept = (l->point.y * l->dir.x - l->point.x * l->dir.y)
			   / l->dir.x;
	struct vector2d l_pt = {
		l->point.x + l->dir.x, 
		l->point.y + l->dir.y
	};
	const double slope = (l_pt.y - intercept) / l_pt.x;
	const double x_extreme = -(the_graph->origin[0] / the_graph->scale[0]);
	const double y_leftmost = slope * x_extreme + intercept;
	const double y_rightmost = intercept - slope * x_extreme;
	cairo_move_to(context, the_graph->dim[0],
		the_graph->origin[1] - y_rightmost * the_graph->scale[1]);
	cairo_line_to(context, 0,
		the_graph->origin[1] - y_leftmost * the_graph->scale[1]);
	cairo_stroke(context);
}

void draw_line_implicit(cairo_t *context, const struct graph *the_graph,
	const struct line2d_implicit *l, const struct color *col,
	const double width) {	
	struct line2d_explicit le;
	to_explicit2d(l, &le);
	draw_line_explicit(context, the_graph, &le, col, width);
}

void draw_vector(cairo_t *context, const struct graph *the_graph,
	const struct vector2d *v, const struct color *col, const double width) {
	cairo_set_source_rgba(context, col->r, col->g, col->b, col->a);
	cairo_set_line_width(context, width);
	cairo_move_to(context, the_graph->origin[0], the_graph->origin[1]);
	cairo_line_to(context, 
		the_graph->origin[0] + v->x * the_graph->scale[0],
		the_graph->origin[1] - v->y * the_graph->scale[1]);
	cairo_stroke(context);
}

void draw_circle(cairo_t *context, const struct graph *the_graph,
	const struct circle2d *c, const struct color *stroke, const double width,
	const struct color *fill) {
	cairo_set_source_rgba(context, stroke->r, stroke->g, stroke->b, stroke->a);
	cairo_set_line_width(context, width);
	// draw an ellipse for scaled circle
	// approximated with Bezier curves
	const double rx = (c->radius) * (the_graph->scale[0]);
	const double ry = (c->radius) * (the_graph->scale[1]);
	const double xmagic = 0.561, ymagic = 0.561;
	const struct vector2d
		ctr = {(the_graph->origin[0]) + (the_graph->scale[0])
			* (c->center.x), (the_graph->origin[1])
			- (the_graph->scale[1]) * (c->center.y)},
		// end points
		p1 = {ctr.x - rx, ctr.y},
		p2 = {ctr.x, ctr.y - ry},
		p3 = {ctr.x + rx, ctr.y},
		p4 = {ctr.x, ctr.y + ry},
		// control points
		c1 = {p1.x, p1.y - ry * ymagic},
		c2 = {p2.x - rx * xmagic, p2.y},
		c3 = {p2.x + rx * xmagic, p2.y},
		c4 = {p3.x, p3.y - ry * ymagic},
		c5 = {p3.x, p3.y + ry * ymagic},
		c6 = {p4.x + rx * xmagic, p4.y},
		c7 = {p4.x - rx * xmagic, p4.y},
		c8 = {p1.x, p1.y + ry * ymagic};
	//perfect circle
	//cairo_arc(context, ctr.x, ctr.y, rx, 0, M_PI * 2);
	//cairo_stroke(context);
	// approx circle
	cairo_move_to(context, p1.x, p1.y);
	cairo_curve_to(context, c1.x, c1.y, c2.x, c2.y, p2.x, p2.y);
	cairo_curve_to(context, c3.x, c3.y, c4.x, c4.y, p3.x, p3.y);
	cairo_curve_to(context, c5.x, c5.y, c6.x, c6.y, p4.x, p4.y);
	cairo_curve_to(context, c7.x, c7.y, c8.x, c8.y, p1.x, p1.y);
	if(fill) {
		cairo_stroke_preserve(context);
		cairo_set_source_rgba(context, stroke->r, stroke->g, stroke->b,
			stroke->a);
		cairo_fill(context);
	} else {
		cairo_stroke(context);
	}
}
