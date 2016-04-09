#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <math.h>
#include <assert.h>
#include <gtk/gtk.h>
#include <glib.h>
#include <GL/glew.h>
#include "gldraw.h"
#include "matrix.h"

static void object_free(DrawObject*);
static void camera_to_perspective(const GLfloat*, const GLfloat,
	const GLfloat, const GLfloat,
	const GLfloat, GLfloat*, GLfloat*);

void init_app_data(AppData *app_data, GLint w, GLint h) {
	bzero(app_data, sizeof(AppData));
	app_data->last_error_cleared = TRUE;
	app_data->app_width = w;
	app_data->app_height = h;
}

DrawObjectStore* object_store_new(DrawObject *first_draw_object) {
	return g_list_append(NULL, first_draw_object);
}

void object_store_free(DrawObjectStore **object_store_ptr) {
	g_list_free_full(*object_store_ptr, object_free);
	*object_store_ptr = NULL;
}

void object_store_add_object(DrawObjectStore **object_store_ptr, 
	const DrawObject *draw_object) {
	*object_store_ptr = g_list_prepend(*object_store_ptr, draw_object);
}

void object_store_remove_object(DrawObjectStore **object_store_ptr, 
	const DrawObject *draw_object) {
	*object_store_ptr = g_list_remove(*object_store_ptr, draw_object);
}

void object_store_for_each(DrawObjectStore *object_store, 
	void (*for_each_func)(DrawObject*, gpointer), gpointer data) {
	g_list_foreach(object_store, for_each_func, data);
}

void shader_from_file(const GLuint shader, const gchar *file_name, AppData *app_data) {
	assert(file_name != NULL);
	gchar *message = NULL;
	size_t fsize = 0, rsize = 0;
	FILE *shader_file = fopen(file_name, "r");
	//check if file exists
	if(!shader_file) {
		size_t file_name_sz = strlen(file_name);
		message = calloc(file_name_sz + 29, sizeof(gchar));
		if(message == NULL)
			g_error("Can't allocate enough memory for error message");
		sprintf(message, "Shader file %s doesn't exist.", file_name);
		app_data->message_printer(
			app_data,
			GTK_MESSAGE_WARNING,
			message
		);
		free(message);
		return;
	}
	//allocate file sized string buffer for reading shader content
	fseek(shader_file, 0L, SEEK_END);
	fsize = ftell(shader_file);
	rewind(shader_file);
	message = calloc(fsize, sizeof(gchar));
	if(message == NULL)
		g_error("Can't allocate enough memory for reading shader file %s", file_name);

	//read the content of shader
	rsize = fread(message, sizeof(gchar), fsize / sizeof(gchar), shader_file);
	if(rsize < fsize)
		app_data->message_printer(
			app_data,
			GTK_MESSAGE_ERROR,
			"Can't read the whole shader file, possible I/O error"
		);
	//done reading file
	fclose(shader_file);

	//set current shader source as the shader just read
	glShaderSource(
		shader,       //the shader ID allocated previously
		1,            //number of strings collectively specifying shader source
		&message,     //the string buffer containing shader source
		NULL          //indicates the buffer is null terminated string
	);

	//free up shader source buffer
	free(message);
}

DrawObject *objects_from_file(const gchar *objects_file, AppData *app_data) {
	assert(objects_file != NULL);
	static FILE *obj_file;
	//static long fsize;
	static gboolean is_camera_lamp_read = FALSE;
	static unsigned int nobjects;
	DrawObject *obj = NULL;
	gchar *message = NULL;
	GLfloat fov, aspect, near, far, cam_transform[16];
	unsigned int vert_arr_sz, norm_arr_sz, attr_arr_sz, shader_arr_sz, shader_fname_sz;
	if(obj_file == NULL) {
		obj_file = fopen(objects_file, "r");
		//check if file exists
		if(!obj_file) {
			size_t file_name_sz = strlen(objects_file);
			message = calloc(file_name_sz + 43, sizeof(gchar));
			if(message == NULL)
				g_error("Can't allocate enough memory for error message");
			sprintf(message, "Object specification file %s doesn't exist.", objects_file);
			app_data->message_printer(
				app_data,
				GTK_MESSAGE_WARNING,
				message
			);
			free(message);
			return obj;
		}
	}
	if(!is_camera_lamp_read) {
		fscanf(obj_file, " Camera Perspective %f %f %f %f", &fov, &aspect, 
			&near, &far);
		fscanf(obj_file, " Transform");
		for(int i = 0; i < 16; ++i) {
			size_t scanned = fscanf(obj_file, " %f", &(cam_transform[i]));
			if(!scanned) break;
		}
		fscanf(obj_file, " Lamp Transform");
		for(int i = 0; i < 16; ++i) {
			size_t scanned = fscanf(obj_file, " %f", &(app_data->lamp[i]));
			if(!scanned) break;
		}
		fscanf(obj_file, " Objects %u", &nobjects);
		is_camera_lamp_read = TRUE;
		camera_to_perspective(cam_transform, fov, aspect, near, far,
			app_data->view, app_data->projection);
	}
	if(nobjects > 0 && fscanf(obj_file, " Object Vertices %u", &vert_arr_sz)) {
		obj = calloc(1, sizeof(DrawObject));
		if(obj == NULL) goto memory_alloc_error;
		obj->vert_sz = vert_arr_sz;
		obj->vertices = calloc(vert_arr_sz, sizeof(GLfloat));
		if(obj->vertices == NULL) goto memory_alloc_error;
		for(int i = 0; i < vert_arr_sz; ++i) {
			size_t scanned = fscanf(obj_file, " %f", &(obj->vertices[i]));
			if(!scanned) break;
		}
		if(fscanf(obj_file, " Normals %u", &norm_arr_sz)) {
			obj->norm_sz = norm_arr_sz;
			obj->normals = calloc(norm_arr_sz, sizeof(GLfloat));
			if(obj->normals == NULL) goto memory_alloc_error;
			for(int i = 0; i < norm_arr_sz; ++i) {
				size_t scanned = fscanf(obj_file, " %f", &(obj->normals[i]));
				if(!scanned) break;
			}
		}
		if(fscanf(obj_file, " Structure %u", &attr_arr_sz)) {
			obj->attrib_list_sz = attr_arr_sz;
			obj->attribute_list = calloc(attr_arr_sz, sizeof(GLint));
			if(obj->attribute_list == NULL) goto memory_alloc_error;
			for(int i = 0; i < attr_arr_sz; ++i) {
				size_t scanned = fscanf(obj_file, " %i", &(obj->attribute_list[i]));
				if(!scanned) break;
			}
		}
		fscanf(obj_file, " Transform");
		for(int i = 0; i < 16; ++i) {
			size_t scanned = fscanf(obj_file, " %f", &(obj->transform[i]));
			if(!scanned) break;
		}
		if(fscanf(obj_file, " Shaders %u", &shader_arr_sz)) {
			obj->shaders_sz = shader_arr_sz;
			obj->shaders = calloc(shader_arr_sz, sizeof(struct file_shader));
			if(obj->shaders == NULL) goto memory_alloc_error;
			for(int i = 0; i < shader_arr_sz; ++i) {
				size_t scanned = fscanf(obj_file, " %u", &shader_fname_sz);
				if(scanned) {
					obj->shaders[i].file_name = calloc(shader_fname_sz + 1, sizeof(gchar));
					if(obj->shaders[i].file_name == NULL) goto memory_alloc_error;
					fscanf(obj_file, " %s %x", obj->shaders[i].file_name,
						&(obj->shaders[i].shader_type));
				} else break;
			}
		}

		//we have read 1 object
		--nobjects;
		//close the file if all the objects are read
		if(nobjects == 0)
			fclose(obj_file);
	}
	return obj;

memory_alloc_error:
	g_error("Can't allocate enough memory to read object data");
}

void print_gl_error(const char *operation, AppData *app_data) {
	//if there is error in last OpenGL operation fetch it
	GLenum err = glGetError();
	if(err != GL_NO_ERROR) {
		//alocate buffer for operation name plus custom message
		size_t operation_sz = strlen(operation);
		gchar *gl_error = calloc(operation_sz + 32, sizeof(char));
		if(gl_error == NULL)
			g_error("Can't allocate enough memory for OpenGL error message");
		sprintf(gl_error, "%s: ", operation);
		const gchar *message = (err == GL_INVALID_ENUM) ? "invalid enumeration" :
				err == GL_INVALID_VALUE ? "invalid value" :
				err == GL_INVALID_OPERATION ? "invalid operation" :
				err == GL_INVALID_FRAMEBUFFER_OPERATION ? "invalid framebuffer operation" :
				err == GL_OUT_OF_MEMORY ? "out of memory" :
				err == GL_STACK_UNDERFLOW ? "stack underflow" :
				err == GL_STACK_OVERFLOW ? "stack overflow" : "unknown error";
		sprintf(gl_error + operation_sz, "%s", message);
		app_data->message_printer(
			app_data,
			GTK_MESSAGE_WARNING,
			gl_error
		);
		//free up message buffer
		free(gl_error);
	}
}

void print_shader_error(const GLuint shader, AppData *app_data) {
	GLint log_max_size = 0, log_size = 0;
	//fetch shader compilation log size
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_max_size);
	if(log_max_size > 0) {
		//alocate buffer for log storage plus custom message,
		//abort on memory allocation error
		GLchar *log_buf = calloc(log_max_size + 26, sizeof(GLchar));
		if(log_buf == NULL)
			g_error("Can't allocate enough memory for shader log");
		//custom message with size 26 characters
		sprintf(log_buf, "Shader compilation error: ");

		//fetch shader compilation log, actual size is returned in "log_size"
		GLchar *log_buf_after_msg = log_buf + 26;
		glGetShaderInfoLog(shader, log_max_size, &log_size, log_buf_after_msg);
		log_buf_after_msg[log_size] = '\0';
		g_print(log_buf);
		//free up log buffer
		free(log_buf);
		//as compilation errors can be large, only indicate that there is error
		app_data->message_printer(
			app_data,
			GTK_MESSAGE_WARNING,
			"Shader compilation error "
		);
	}
}

void object_free(DrawObject *draw_object) {
	if(draw_object->vertices && draw_object->vert_sz > 0)
		free(draw_object->vertices);
	if(draw_object->normals && draw_object->norm_sz > 0)
		free(draw_object->normals);
	if(draw_object->attribute_list && draw_object->attrib_list_sz > 0)
		free(draw_object->attribute_list);
	if(draw_object->shaders && draw_object->shaders_sz > 0) {
		for(int i = 0; i < draw_object->shaders_sz; ++i)
			free(draw_object->shaders[i].file_name);
		free(draw_object->shaders);
	}
	free(draw_object);
}

void camera_to_perspective(const GLfloat *camera_transform,
	const GLfloat fov, const GLfloat aspect,
	const GLfloat near, const GLfloat far, GLfloat *view, GLfloat *projection) {
#define M_PI 3.14159265358979323846f
	//width and height of the viewport
	GLfloat ratio = 1.0f / tan(fov / 2.0f * M_PI / 180.0f);
	projection[0] = ratio / aspect;
	projection[5] = ratio ;
	projection[10] = (far + near) / (near - far);
	projection[11] = 2 * far * near / (near - far);
	projection[14] = -1;
	//invert camera transform to obtain view matrix
	mat4f_inverse(camera_transform, view);
#undef M_PI
}
