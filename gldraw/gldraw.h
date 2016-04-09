#ifndef GLDRAW_H
#define GLDRW_H

#include <GL/glew.h>
#include <gtk/gtk.h>
#include <gtk/gtkgl.h>
#include <glib.h>

struct file_shader {
	gchar *file_name;
	GLenum shader_type;
};

typedef struct _draw_object {
	GLfloat *vertices, *normals;   //vertex and surface normal values of the object
	GLuint *attribute_list;         //sequence of vertices and normals used per polygon
	GLfloat transform[16];         //a 4x4 column major LocRotScale matrix
	struct file_shader *shaders;   //all shaders attached to this object
	//lengths of buffers
	size_t vert_sz, norm_sz, attrib_list_sz, shaders_sz;
} DrawObject;

//minimal scene graph implemented with doubly linked list
typedef GList DrawObjectStore;

typedef struct _app_data AppData;
struct _app_data {
	GLint app_width, app_height;
	GtkInfoBar *app_message_area;  //required to convey errors and info in-app
	GtkLabel *app_message;         //required to convey errors and info in-app
	gboolean last_error_cleared;   //required for keeping track of user acknowledgement on display of error message
	void (*message_printer)(AppData*, const GtkMessageType, const gchar*);
	GdkGLDrawable *gl_drawing_area;//required for GtkGLExt draw calls
	GdkGLContext *gl_context;      //required for GtkGLExt draw calls
	DrawObjectStore *draw_objects; //required for storing objects to be drawn
	GLfloat view[16],              //view matrix
	        projection[16],        //projection matrix
	        lamp[16];              //lamp position
};

void init_app_data(AppData*, GLint, GLint);
void print_gl_error(const char *, AppData*);
void print_shader_error(const GLuint, AppData*);
void shader_from_file(const GLuint, const gchar*, AppData*);
DrawObject* objects_from_file(const gchar*, AppData*);
void object_draw(DrawObject*, GLuint, AppData*);
DrawObjectStore* object_store_new(DrawObject*);
void object_store_free(DrawObjectStore**);
void object_store_add_object(DrawObjectStore**, const DrawObject*);
void object_store_remove_object(DrawObjectStore**, const DrawObject*);
void object_store_for_each(DrawObjectStore*, void (*)(DrawObject*, gpointer), gpointer);
void draw_all(AppData*);

#endif
