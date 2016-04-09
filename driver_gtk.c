#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <assert.h>
#include <gtk/gtk.h>
#include "vector.h"
#include "line.h"
#include "circle.h"
#include "helper.h"
const struct color gray = {0.5, 0.5, 0.5, 1};
const struct color red = {1, 0, 0, 1};
const struct color green = {0, 1, 0, 1};
const struct color blue = {0, 0, 1, 1};
const struct color purple = {0.8, 0, 1, 1};
const struct line2d_explicit le = {
	.point = {2, 3},
	.dir = {4, 5},
	.param = 2
};
const struct line2d_implicit li = {
	.normal = {4, 2},
	.param = -7
};
const struct circle2d c = {
	.center = {3, 2},
	.radius = 4
};
const struct vector2d vec = {4, 5};
const struct vector2d origin = {0, 0};
const gint h = 600, w = 600, pane_width = 200;

static void init_graph(struct graph *g, double w, double h) {
	g->dim[0] = w;
	g->dim[1] = h;
	g->origin[0] = w / 2;
	g->origin[1] = h / 2;
	g->scale[0] = w / 20;
	g->scale[1] = h / 20;
	g->grid_line_width = 0.5;
	g->axes_line_width = 1;
	g->grid_col = &gray;
	g->axes_col = &gray;
}

static void on_destroy(GtkWidget *window, gpointer data) {
	gtk_main_quit();
}

static gboolean on_draw(GtkWidget *window, cairo_t *context, gpointer graph) {	
	assert(context != NULL);
	gint wd, ht;
	struct graph *g = graph;
	wd = gtk_widget_get_allocated_width(window);
	ht = gtk_widget_get_allocated_height(window);
	init_graph(g, wd, ht);
	draw_grid_axes(context, g);
	draw_vector(context, g, &vec, &blue, 1);
	draw_line_explicit(context, g, &le, &red, 1);
	draw_line_implicit(context, g, &li, &green, 1);
	draw_circle(context, g, &c, &purple, 1, NULL);
	
	draw_vector(context, g, &(le.dir), &blue, 1);
	draw_vector(context, g, &(li.normal), &blue, 1);

	unsigned char is_parallel;
	struct vector2d le_li_pt = find_intersect2d(&le, &li, &is_parallel);
	struct vector2d li_c_n_pt = point_to_line_nearest2d(&(c.center), &li);
	struct vector2d li_o_n_pt = point_to_line_nearest2d(&origin, &li);
	draw_point(context, g, &(c.center), &purple);
	if(!is_parallel) draw_point(context, g, &le_li_pt, &blue);
	draw_point(context, g, &li_c_n_pt, &purple);
	draw_point(context, g, &li_o_n_pt, &gray);

	struct line2d_implicit le_to_li;
	to_implicit2d(&le, &le_to_li);
	struct vector2d le_c_n_pt = point_to_line_nearest2d(&(c.center), &le_to_li);
	struct vector2d le_o_n_pt = point_to_line_nearest2d(&origin, &le_to_li);
	draw_point(context, g, &le_c_n_pt, &purple);
	draw_point(context, g, &le_o_n_pt, &gray);

	return TRUE;
}

int main(int argc, char *argv[]) {
	int i, j;
	const char *name = "vector-test";
	struct graph g;
	init_graph(&g, w, h);
	gtk_init(&argc, &argv);
	GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_widget_set_size_request(window, w + pane_width, h);
	gtk_window_set_title(GTK_WINDOW(window), name);
	g_signal_connect(window, "destroy", G_CALLBACK(on_destroy), NULL);

	GtkWidget *row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_box_set_homogeneous(GTK_BOX(row), FALSE);
	GtkWidget *col = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	GtkWidget *mtable = gtk_grid_new();
	gtk_grid_set_row_homogeneous(GTK_GRID(mtable), TRUE);
	gtk_grid_set_column_homogeneous(GTK_GRID(mtable), TRUE);
	GtkWidget *matrix[2][2];
	for(i = 0; i < 2; ++i) {
		for(j = 0; j < 2; ++j) {
			matrix[i][j] = gtk_entry_new();
			gtk_grid_attach(GTK_GRID(mtable), GTK_WIDGET(matrix[i][j]), 
				i, j, 1, 1);
		}
	}
	gtk_box_pack_start(GTK_BOX(col), mtable, TRUE, TRUE, 20);
	GtkWidget *apply_btn = gtk_button_new_with_mnemonic("_Apply matrix");
	gtk_box_pack_end(GTK_BOX(col), apply_btn, TRUE, FALSE, 20);
	gtk_box_pack_end(GTK_BOX(row), col, FALSE, FALSE, 0);

	GtkWidget *drawing_area = gtk_drawing_area_new();
	gtk_widget_set_size_request(drawing_area, w, h);
	gtk_box_pack_start(GTK_BOX(row), drawing_area, TRUE, TRUE, 0);
	gtk_container_add(GTK_CONTAINER(window), row);

	g_signal_connect(drawing_area, "draw", G_CALLBACK(on_draw), &g);
	gtk_widget_show_all(window);

	gtk_main();
	return 0;
}
