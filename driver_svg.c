#include <math.h>
#include <stdlib.h>
#include "vector.h"
#include "line.h"
#include "helper.h"

int main(void) {
	const struct color c_ax_gd = {0.5, 0.5, 0.5, 1};
	const struct color c_l = {1, 0, 0, 1};
	const struct color c_v = {0, 0.5, 0.5, 1};
	const struct line2d_explicit le = {
		.point = {2, 3},
		.dir = {4, 5},
		.param = 2
	};
	struct line2d_implicit li = {.param = 4};
	struct vector2d vec = {4, 5};
	const double h = 600, w = 800;
	const char *file = "vector-test.svg";
	const struct graph g = { 
		.origin = {w / 2, h / 2},
		.dim = {w, h},
		.scale = {w / 20, h / 20},
		.grid_line_width = 0.5,
		.axes_line_width = 1,
		.grid_col = &c_ax_gd,
		.axes_col = &c_ax_gd
	};

	int exit_status = EXIT_SUCCESS;
	struct cairo_resources res;
	if(init_cairo_svg(file, &g, &res)) {
		draw_grid_axes(res.context, &g);
		draw_line_explicit(res.context, &g, &le, &c_l, 1);
		draw_vector(res.context, &g, &vec, &c_v, 1);
		vec = normal2d(vec);
		draw_vector(res.context, &g, &vec, &c_v, 1);
		vec = rotate2d(M_PI / 3, vec);
		draw_vector(res.context, &g, &vec, &c_v, 1);
		li.normal = vec;
		draw_line_implicit(res.context, &g, &li, &c_l, 1);
	} else {
		exit_status = EXIT_FAILURE;
	}
	cleanup_cairo(&res);
	return exit_status;
}
