#include <stdlib.h>
#include <GL/glew.h>
#include <gtk/gtk.h>
#include "gldraw.h"

struct draw_object_struct {
	GLuint ubo, shader_prog;
	AppData *app_data;
};
static void draw_object_func(DrawObject*, gpointer);

void draw_all(AppData *app_data) {
	GLuint ubo;
	struct draw_object_struct ds = {};

	//initialize glew for reliable OpenGL extension loading
	if(glewInit() != GLEW_OK)
		app_data->message_printer(
			app_data,
			GTK_MESSAGE_ERROR,
			"Can't initialize GLEW"
		);

	//setup viewport and background color
	glViewport(0, 0, app_data->app_width, app_data->app_height);
	glClearColor(0.235f, 0.231f, 0.215f, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	//enable depth test
	glEnable(GL_DEPTH_TEST);
	glClear(GL_DEPTH_BUFFER_BIT);

	//enable antialiasing
	/*glEnable(GL_BLEND);
	glEnable(GL_POLYGON_SMOOTH);
	glBlendFunc(GL_SRC_ALPHA, GL_DST_ALPHA);*/

	if(app_data->draw_objects != NULL) {
		ds.app_data = app_data;
		object_store_for_each(app_data->draw_objects, draw_object_func, &ds);
	}
}

//ubo is uniform buffer object allocated once for each frame to store transforms
void object_draw(DrawObject *draw_object, GLuint ubo, AppData *app_data) {
	//layout positions of shader attributes
	static gboolean is_shader_program_initialized = FALSE;
	GLuint index_vertices   = 0,
	       index_normals    = 1;
	const GLchar *attrib_vertices    = "gldraw_vert",  //attribute name for vertex locations in shader
	             *attrib_normals     = "gldraw_norm",  //attribute name for surface normals in shader
	             *uniform_model      = "gldraw_model"; //uniform containing model transform
	const GLchar *uniforms[] = {"gldraw_lamp", "gldraw_view", "gldraw_projection"};
	GLuint indices_uniform[3];
	GLuint vbo, shader_prog, elem_indices, index_model, *shaders;
	GLfloat *combined_vert_buf = NULL;

	//create interleaved array of vertex positions and normals
	combined_vert_buf = calloc(draw_object->attrib_list_sz * 3, sizeof(GLfloat));
	if(combined_vert_buf == NULL)
		g_error("can't allocate memory for combined array ");
	for(int i = 0; i < draw_object->attrib_list_sz; i += 2) {
		//vertex position X component
		combined_vert_buf[i * 3] = draw_object->vertices[
			draw_object->attribute_list[i] * 3];
		//vertex position Y component
		combined_vert_buf[i * 3 + 1] = draw_object->vertices[
			draw_object->attribute_list[i] * 3 + 1];
		//vertex position Z component
		combined_vert_buf[i * 3 + 2] = draw_object->vertices[
			draw_object->attribute_list[i] * 3 + 2];
		//vertex normal X component
		combined_vert_buf[(i + 1) * 3] = draw_object->normals[
			draw_object->attribute_list[i + 1] * 3];
		//vertex normal Y component
		combined_vert_buf[(i + 1) * 3 + 1] = draw_object->normals[
			draw_object->attribute_list[i + 1] * 3 + 1];
		//vertex normal Z component
		combined_vert_buf[(i + 1) * 3 + 2] = draw_object->normals[
			draw_object->attribute_list[i + 1] * 3 + 2];
	}

	//allocate space for and upload vertex and normal data
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(
		GL_ARRAY_BUFFER,                            //this buffer contains vertex data for draw calls
		                                            //this buffer will contain vertex location and surface normals
		(draw_object->attrib_list_sz) * 3 * sizeof(GLfloat),//X, Y, Z comeponents
		combined_vert_buf,                          //application space buffer
		GL_STATIC_DRAW                              //filled once, read many times
	);
	print_gl_error("error uploading vertex and normals to VBO:", app_data);

	//enable vertex and normal attributes
	glEnableVertexAttribArray(index_vertices);
	glEnableVertexAttribArray(index_normals);

	//specify vertex buffer format
	glVertexAttribPointer(
		index_vertices,                             //specifying format of vertex position attribute
		3,                                          //X, Y, Z = 3 components per attribute
		GL_FLOAT,                                   //all compoents are float type
		GL_FALSE,                                    //data uploaded was not normalized
		6 * sizeof(GLfloat),                        //byte offset between this and nex attribute, space for normals
		0                                           //first element of the array
	);
	glVertexAttribPointer(
		index_normals,                              //specifying format of vertex normal attribute
		3,                                          //X, Y, Z = 3 components per attribute
		GL_FLOAT,                                   //all compoents are float type
		GL_FALSE,                                    //data uploaded was not normalized
		6 * sizeof(GLfloat),                        //byte offset between this and next attribute, space for positions
		(GLchar *)NULL + 3 * sizeof(GLfloat)        //2nd element of the array = XYZ of 1st position
	);

	//free up this huge application space buffer
	free(combined_vert_buf);
	
	//apply shaders
	shaders = calloc(draw_object->shaders_sz, sizeof(GLuint));
	if(shaders == NULL)
		g_error("can't allocate memory for storing shader handles ");
	//create shader program object and attach default shaders
	shader_prog = glCreateProgram();
	print_gl_error("error creating program object:", app_data);
	for(int i = 0; i < draw_object->shaders_sz; ++i) {
		shaders[i] = glCreateShader(draw_object->shaders[i].shader_type);
		print_gl_error("error creating shader:", app_data);
		shader_from_file(shaders[i], draw_object->shaders[i].file_name, app_data);
		glCompileShader(shaders[i]);
		print_gl_error("error compiling shader:", app_data);
		print_shader_error(shaders[i], app_data);
		glAttachShader(shader_prog, shaders[i]);
		print_gl_error("error attaching shaders to program:", app_data);
	}
	glLinkProgram(shader_prog);
	print_gl_error("error linking shaders into program:", app_data);

	//apply shaders to current object
	glUseProgram(shader_prog);
	print_gl_error("error applying shaders:", app_data);

	//bind shader variables to specified locations
	glBindAttribLocation(shader_prog, index_vertices, attrib_vertices);
	glBindAttribLocation(shader_prog, index_normals, attrib_normals);
	if(!is_shader_program_initialized) {
		//upload shader uniforms once: lamp, view projection
		//first fetch the location of uniforms
		indices_uniform[0] = glGetUniformLocation(shader_prog, uniforms[0]);//lamp
		indices_uniform[1] = glGetUniformLocation(shader_prog, uniforms[1]);//view
		indices_uniform[2] = glGetUniformLocation(shader_prog, uniforms[2]);//projection
		//then upload at fetched locations
		glUniformMatrix4fv(
			indices_uniform[0],                    //location of lamp in shader
			1,                                     //modify single uniform variable
			GL_TRUE,                               //row-major format
			app_data->lamp                         //lamp transform
		);
		glUniformMatrix4fv(
			indices_uniform[1],                    //location of view in shader
			1,                                     //modify single uniform variable
			GL_TRUE,                               //row-major format
			app_data->view                         //view transform
		);
		glUniformMatrix4fv(
			indices_uniform[2],                    //location of projection in shader
			1,                                     //modify single uniform variable
			GL_TRUE,                               //row-major format
			app_data->projection                   //projection transform
		);
		print_gl_error("error binding lamp, view and projection transforms to shader uniforms:", app_data);
		is_shader_program_initialized = TRUE;
	}
	//upload shader uniform for each model
	index_model = glGetUniformLocation(shader_prog, uniform_model);
	glUniformMatrix4fv(
		index_model,                               //location of model transform in shader
		1,                                         //modify single uniform variable
		GL_TRUE,                                   //row-major format
		draw_object->transform                     //lamp transform
	);
	print_gl_error("error binding model transform with shader uniform:", app_data);

	//draw
	glDrawArrays(
		GL_TRIANGLES,                               //render model as collection of triangles
		0,                                          //start from 1st element
		(draw_object->attrib_list_sz / 2)           //number of elements 
	);

	//free up resources
	for(int i = 0; i < draw_object->shaders_sz; ++i) {
		glDeleteShader(shaders[i]);
	}
	free(shaders);
	glDeleteProgram(shader_prog);
	glInvalidateBufferData(vbo);
	glDeleteBuffers(1, &vbo);
}

void draw_object_func(DrawObject *draw_object, gpointer data) {
	struct draw_object_struct *ds = data;
	object_draw(draw_object, ds->ubo, ds->app_data);
}
