#include <assert.h>
#include <gtk/gtk.h>
#include <gtk/gtkgl.h>
#include "gldraw.h"

static void on_appwindow_close(GtkWidget*, gpointer);
static void gl_context_init(GtkWidget*, GtkWidget*, AppData*);
static gboolean on_drawing_area_expose(GtkWidget*, GdkEvent*, gpointer);
static void on_app_message_area_response(GtkInfoBar*, gint, gpointer);
static void app_show_message(AppData*, const GtkMessageType, const gchar*);

int main(int argc, char **argv) {
	const gchar *app_title = "GLDraw";
	const gint app_width = 640, app_height = 480;
	GtkWidget *app_window = NULL, *vbox = NULL, *drawing_area = NULL;
	const gchar *objects_file = "gldraw_objects.txt";
	AppData app_data;
	init_app_data(&app_data, app_width, app_height);
	
	//initialize GTK+ and create widgets
	gtk_init(&argc, &argv);
	app_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	vbox = gtk_vbox_new(FALSE, 0);          //non-homogeneous and 0 spacing between elements
	drawing_area = gtk_drawing_area_new();
	app_data.app_message_area = GTK_INFO_BAR(gtk_info_bar_new_with_buttons(
		GTK_STOCK_OK,                       //add OK button to info bar
		GTK_RESPONSE_OK,                    //"response" signal emitted when pressed
		NULL
	));
	app_data.app_message = GTK_LABEL(gtk_label_new(""));

	//initialize GtkGLExt
	gtk_gl_init(&argc, &argv);

	//configure main window
	gtk_window_set_title(GTK_WINDOW(app_window), app_title);
	gtk_window_set_default_size(GTK_WINDOW(app_window), app_width, app_height);
	gtk_container_add(GTK_CONTAINER(app_window), vbox);
	// main window signal handlers
	g_signal_connect(
		G_OBJECT(app_window),               //event target
	    "destroy",                          //event type
	    G_CALLBACK(on_appwindow_close),     //event handler
	    &app_data                           //additional data to be passed to
	);                                      //event handler

	//configure vbox with drawing area and info bar as children
	gtk_box_pack_start(
		GTK_BOX(vbox),
		GTK_WIDGET(app_data.app_message_area),
		FALSE,                              //don't expand
		FALSE,                              //don't fill(ignored)
		0                                   //no spaning between children
	);
	gtk_box_pack_start(
		GTK_BOX(vbox),
		drawing_area,
		TRUE,                              //expand to parent
		TRUE,                              //fill
		0                                  //no spaning between children
	);

	//configure info bar
	gtk_misc_set_alignment(GTK_MISC(app_data.app_message), 0, 0.5);//aligns label text to left
	gtk_container_add(
		GTK_CONTAINER(gtk_info_bar_get_content_area(app_data.app_message_area)),
		GTK_WIDGET(app_data.app_message)
	);
	//function to print error messages in info bar
	app_data.message_printer = app_show_message;
	//info bar signal handlers
	g_signal_connect(
		G_OBJECT(app_data.app_message_area),//hide info bar when OK is pressed
		"response",
		G_CALLBACK(on_app_message_area_response),
		&app_data
	);

	//initialize OpenGL context in attached window
	gl_context_init(app_window, drawing_area, &app_data);

	//drawing area signal handlers
	g_signal_connect(
		G_OBJECT(drawing_area),             //called when drawing area
	    "expose-event",                     //is redrawn
	    G_CALLBACK(on_drawing_area_expose),
	    &app_data
	);

	//try to read an object from objects file to see if it contains any data
	DrawObject *first_object = objects_from_file(objects_file, &app_data);
	if(first_object) {
		//allocate object store
		app_data.draw_objects = object_store_new(first_object);
		//read rest of the objects from the file
		while(first_object = objects_from_file(objects_file, &app_data)) {
			object_store_add_object(&(app_data.draw_objects), first_object);
		}
	}

	//show everything in the the main window
	gtk_widget_show_all(app_window);

	//enter GTK+ main loop and wait for its return
	gtk_main();

	return 0;
}

void on_appwindow_close(GtkWidget *app_window_widget, gpointer data) {
	AppData *app_data = data;
	//free up object store
	object_store_free(&(app_data->draw_objects));
	
	gtk_main_quit();
}

void gl_context_init(GtkWidget *app_window_widget, 
	GtkWidget *drawing_area_widget, AppData *app_data) {
	//create OpenGL framebuffer configuration object
	GdkGLConfig *gl_fb_config = gdk_gl_config_new_by_mode(
		GDK_GL_MODE_RGB    |                //support colour
		GDK_GL_MODE_DOUBLE |                //support double buffering
		GDK_GL_MODE_ALPHA  |                //support alpha blending
		GDK_GL_MODE_DEPTH                   //support depth tests
	);
	assert(gl_fb_config != NULL);
	
	//try to make drawing area OpenGL capable
	//this MUST happen BEFORE drawing area is realized
	if(!gtk_widget_set_gl_capability(
		drawing_area_widget,
		gl_fb_config,
		NULL,                               //doesn't share display list and texture objects with another widget
		TRUE,                               //direct rendering
		GDK_GL_RGBA_TYPE                    //unused constant parameter craft
	))
		app_show_message(
			app_data,
			GTK_MESSAGE_ERROR,
			"Can't make drawing area OpenGL capable"
		);
	//since GtkGLExt can't initialize without realized widgets
	//show all the contents of main window and initialize it(except the info bar)
	gtk_widget_set_no_show_all(GTK_WIDGET(app_data->app_message_area), TRUE);
	gtk_widget_show_all(app_window_widget);
	//obtain OpenGL context and drawing area from the widget
	//this MUST happen AFTER drawing area is realized
	app_data->gl_drawing_area = gtk_widget_get_gl_drawable(drawing_area_widget);
	app_data->gl_context = gtk_widget_get_gl_context(drawing_area_widget);
	assert(app_data->gl_context != NULL);

	//release framebuffer configuration object
	g_object_unref(G_OBJECT(gl_fb_config));
}

gboolean on_drawing_area_expose(GtkWidget *drawing_area_widget,
	GdkEvent *expose, gpointer data) {
	AppData *app_data = data;
	//OpenGL context activation by GtkGLExt
	gdk_gl_drawable_gl_begin(app_data->gl_drawing_area, app_data->gl_context);

	//OpenGL drawing function call
	draw_all(app_data);

	//swap front and back buffer of drawing area to update it
	gdk_gl_drawable_swap_buffers(app_data->gl_drawing_area);

	gdk_gl_drawable_gl_end(app_data->gl_drawing_area);
	return TRUE;
}

void on_app_message_area_response(GtkInfoBar *message_area, 
	gint response_id, gpointer data) {
	AppData *app_data = data;
	//user has manually acknowledged the error message
	//now further messages can be shown
	app_data->last_error_cleared = TRUE;
	gtk_widget_hide(GTK_WIDGET(message_area));
}

void app_show_message(AppData *app_data, const GtkMessageType msg_type, 
	const gchar *msg) {
	//print error messages to terminal and don't show new errors
	//if user has not canceled last error message.
	//this is to prevent spurious errors from drawing area resize events
	//it also prevents trying to print in GUI if it has not been realized
	GtkWidget *main_window = gtk_widget_get_toplevel(
		GTK_WIDGET(app_data->app_message_area)
	);
	if(app_data->last_error_cleared && gtk_widget_get_realized(main_window)) {
		gtk_label_set_text(app_data->app_message, msg);
		gtk_info_bar_set_message_type(app_data->app_message_area, msg_type);
		gtk_widget_show(GTK_WIDGET(app_data->app_message_area));
		gtk_widget_show(GTK_WIDGET(app_data->app_message));
		app_data->last_error_cleared = FALSE;
	} else
		g_print("%s\n", msg);
}
