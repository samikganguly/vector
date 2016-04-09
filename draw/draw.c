#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <gtk/gtk.h>
#include "draw_types.h"
#include "draw_types.c"

static DrawToolCircle circle_tool;
static DrawToolLineSegment line_tool;

static inline void geometry_draw(cairo_t *ctx, DrawApp *draw_app) {
	if(draw_app->current_tool)
		draw_app->current_tool->draw(draw_app->current_tool, ctx,
			draw_app->last_btn_x, draw_app->last_btn_y,
			draw_app->curr_btn_x, draw_app->curr_btn_y);
}

static inline
void on_is_line_col_toggled(GtkToggleButton *is_line_col, gpointer data) {
	DrawApp *draw_app = (DrawApp *)data;
	if(draw_app->current_tool) {
		draw_app->current_tool->line = gtk_toggle_button_get_active(is_line_col);
		gtk_widget_queue_draw(draw_app->drawing_area);
	}
}

static inline 
void on_is_fill_col_toggled(GtkToggleButton *is_fill_col, gpointer data) {
	DrawApp *draw_app = (DrawApp *)data;
	if(draw_app->current_tool) {
		draw_app->current_tool->fill = gtk_toggle_button_get_active(is_fill_col);
		gtk_widget_queue_draw(draw_app->drawing_area);
	}
}

static inline void on_line_col_set(GtkColorButton *line_col, gpointer data) {
	DrawApp *draw_app = (DrawApp *)data;
	if(draw_app->current_tool) {
		gtk_color_chooser_get_rgba(GTK_COLOR_CHOOSER(line_col),
			draw_app->current_tool->line_col);
		gtk_widget_queue_draw(draw_app->drawing_area);
	}
}

static inline void on_fill_col_set(GtkColorButton *fill_col, gpointer data) {
	DrawApp *draw_app = (DrawApp *)data;
	if(draw_app->current_tool) {
		gtk_color_chooser_get_rgba(GTK_COLOR_CHOOSER(fill_col),
			draw_app->current_tool->fill_col);
		gtk_widget_queue_draw(draw_app->drawing_area);
	}
}

static void on_circle_btn_toggled(GtkToggleButton *circle_btn, gpointer data) {
	assert(data != NULL);
	DrawApp *draw_app = (DrawApp *)data;
	if(gtk_toggle_button_get_active(circle_btn)) {
		draw_app->current_tool = (DrawTool *)(&circle_tool);
		gtk_label_set_label(draw_app->label1,
			draw_app->current_tool->prop1_label);
		gtk_label_set_label(draw_app->label2,
			draw_app->current_tool->prop2_label);
		gchar prop1_val[15], prop2_val[15];
		sprintf(prop1_val, "(%.2lf,%.2lf)", draw_app->last_btn_x, draw_app->last_btn_y);
		sprintf(prop2_val, "(%.2lf,%.2lf)", draw_app->curr_btn_x, draw_app->curr_btn_y);
		gtk_entry_set_text(draw_app->prop1, prop1_val);
		gtk_entry_set_text(draw_app->prop2, prop2_val);
	} else {
		draw_app->current_tool = NULL;
	}
	gtk_widget_queue_draw(draw_app->drawing_area);
}

static void on_line_btn_toggled(GtkToggleButton *line_btn, gpointer data) {
	assert(data != NULL);
	DrawApp *draw_app = (DrawApp *)data;
	if(gtk_toggle_button_get_active(line_btn)) {
		draw_app->current_tool = (DrawTool *)(&line_tool);
		gtk_label_set_label(draw_app->label1,
			draw_app->current_tool->prop1_label);
		gtk_label_set_label(draw_app->label2,
			draw_app->current_tool->prop2_label);
		gchar prop1_val[15], prop2_val[15];
		sprintf(prop1_val, "(%.2lf,%.2lf)", draw_app->last_btn_x, draw_app->last_btn_y);
		sprintf(prop2_val, "(%.2lf,%.2lf)", draw_app->curr_btn_x, draw_app->curr_btn_y);
		gtk_entry_set_text(draw_app->prop1, prop1_val);
		gtk_entry_set_text(draw_app->prop2, prop2_val);
	} else {
		draw_app->current_tool = NULL;
	}
	gtk_widget_queue_draw(draw_app->drawing_area);
}

static
gboolean on_draw(GtkWidget *drawing_area, cairo_t *context, gpointer data) {
	DrawApp *draw_app = (DrawApp *)data;
	geometry_draw(context, draw_app);
	return TRUE;
}

static
gboolean on_draw_btn_press(GtkWidget *drawing_area, GdkEvent *evt, gpointer data) {
	DrawApp *draw_app = (DrawApp *)data;
	GdkEventButton *btn_evt = (GdkEventButton *)evt;
	draw_app->last_btn_x = btn_evt->x;
	draw_app->last_btn_y = btn_evt->y;
	draw_app->last_btn_press_valid = TRUE;
	gchar prop1_val[15];
	sprintf(prop1_val, "(%.2lf,%.2lf)", draw_app->last_btn_x, draw_app->last_btn_y);
	gtk_entry_set_text(draw_app->prop1, prop1_val);
	return TRUE;
}

static
gboolean on_draw_btn_release(GtkWidget *drawing_area, GdkEvent *evt, gpointer data) {
	DrawApp *draw_app = (DrawApp *)data;
	GdkEventButton *btn_evt = (GdkEventButton *)evt;
	if(draw_app->last_btn_press_valid) {
		draw_app->curr_btn_x = btn_evt->x;
		draw_app->curr_btn_y = btn_evt->y;
		draw_app->last_btn_press_valid = FALSE;
		gchar prop2_val[15];
		sprintf(prop2_val, "(%.2lf,%.2lf)", draw_app->curr_btn_x, draw_app->curr_btn_y);
		gtk_entry_set_text(draw_app->prop2, prop2_val);
		gtk_widget_queue_draw(drawing_area);
	}
	return TRUE;
}

static void on_app_startup(GApplication *app, gpointer data) {
	DrawApp *draw_app = (DrawApp *)data;
	GError *res_load_err = NULL;
	GResource *app_res = g_resource_load("draw-resource.gresource",
							&res_load_err);
	if(!app_res) {
		g_critical("Failed to load application resources: %s\n", 
							res_load_err->message);
		g_error_free(res_load_err);
		exit(EXIT_FAILURE);
	}
	draw_app->app_res = app_res;
	g_resources_register(app_res);
	draw_tool_circle_init(&circle_tool);
	draw_tool_line_init(&line_tool);
}

static void on_app_activate(GApplication *app, gpointer data) {
	const gint drawing_area_w = 600, drawing_area_h = 600;
	DrawApp *draw_app = (DrawApp *)data;

	GtkBuilder *app_builder = gtk_builder_new_from_resource(
	                          "/org/samik/draw/draw.glade");
	GObject *main_window = gtk_builder_get_object(app_builder, "main_window");
	GObject *draw_frame = gtk_builder_get_object(app_builder, 
		"draw_area_frame_align");
	GObject *is_line_color = gtk_builder_get_object(app_builder,
		"is_line_color");
	GObject *is_fill_color = gtk_builder_get_object(app_builder,
		"is_fill_color");
	GObject *line_color_chooser = gtk_builder_get_object(app_builder,
		"line_color_chooser_btn");
	GObject *fill_color_chooser = gtk_builder_get_object(app_builder,
		"fill_color_chooser_btn");
	GObject *circle_btn = gtk_builder_get_object(app_builder, "circle_button");
	GObject *line_btn = gtk_builder_get_object(app_builder, "line_button");
	GObject *label_tool_prop1 = gtk_builder_get_object(app_builder,
			"label_tool_prop_1");
	GObject *label_tool_prop2 = gtk_builder_get_object(app_builder,
			"label_tool_prop_2");
	GObject *tool_prop1 = gtk_builder_get_object(app_builder,
			"tool_prop_1");
	GObject *tool_prop2 = gtk_builder_get_object(app_builder,
			"tool_prop_2");
	if(!main_window || !draw_frame || !is_line_color || !is_fill_color
		|| !line_color_chooser || !fill_color_chooser || !circle_btn 
		|| !line_btn) {
		g_critical("Failed to load proper UI structure\n");
		exit(EXIT_FAILURE);
	}
	draw_app->label1 = GTK_LABEL(label_tool_prop1);
	draw_app->label2 = GTK_LABEL(label_tool_prop2);
	draw_app->prop1 = GTK_ENTRY(tool_prop1);
	draw_app->prop2 = GTK_ENTRY(tool_prop2);
	g_signal_connect(GTK_WIDGET(is_line_color), "toggled",
		G_CALLBACK(on_is_line_col_toggled), draw_app);
	g_signal_connect(GTK_WIDGET(is_fill_color), "toggled",
		G_CALLBACK(on_is_fill_col_toggled), draw_app);
	g_signal_connect(GTK_WIDGET(line_color_chooser), "color-set",
		G_CALLBACK(on_line_col_set), draw_app);
	g_signal_connect(GTK_WIDGET(fill_color_chooser), "color-set",
		G_CALLBACK(on_fill_col_set), draw_app);
	g_signal_connect(GTK_WIDGET(circle_btn), "toggled",
		G_CALLBACK(on_circle_btn_toggled), draw_app);
	g_signal_connect(GTK_WIDGET(line_btn), "toggled",
		G_CALLBACK(on_line_btn_toggled), draw_app);

	GtkWidget *drawing_area = gtk_drawing_area_new();
	draw_app->drawing_area = drawing_area;
	gtk_widget_set_size_request(drawing_area, drawing_area_w, drawing_area_h);
	gtk_widget_add_events(drawing_area, 
		GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK);
	g_signal_connect(drawing_area, "button-press-event",
		G_CALLBACK(on_draw_btn_press), draw_app);
	g_signal_connect(drawing_area, "button-release-event",
		G_CALLBACK(on_draw_btn_release), draw_app);
	g_signal_connect(drawing_area, "draw", G_CALLBACK(on_draw), draw_app);
	gtk_container_add(GTK_CONTAINER(draw_frame), drawing_area);
	gtk_window_set_application(GTK_WINDOW(main_window), GTK_APPLICATION(app));
	gtk_widget_show_all(GTK_WIDGET(main_window));
}

static void on_app_shutdown(GApplication *app, gpointer data) {
	DrawApp *draw_app = (DrawApp *) data;
	//if(draw_app->drawing_area) g_object_unref(draw_app->drawing_area);
	if(draw_app->app_res) {
		g_resources_unregister(draw_app->app_res);
		g_resource_unref(draw_app->app_res);
	}
}

int main(int argc, char *argv[]) {
	int status;
	DrawApp draw_app;
	draw_app_init(&draw_app);

	GtkApplication *app = gtk_application_new("org.samik.draw", 
							G_APPLICATION_FLAGS_NONE);
	g_signal_connect(app, "startup", G_CALLBACK(on_app_startup), &draw_app);
	g_signal_connect(app, "activate", G_CALLBACK(on_app_activate), &draw_app);
	g_signal_connect(app, "shutdown", G_CALLBACK(on_app_shutdown), &draw_app);
	status = g_application_run(G_APPLICATION(app), argc, argv);

	g_object_unref(app);

	return status;
}
