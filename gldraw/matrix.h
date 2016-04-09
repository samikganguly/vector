#ifndef MATRIX_H
#define MATRIX_H

#include <GL/glew.h>

typedef GLfloat mat4f[4][4];
typedef GLfloat vec4f[4];

void mat4f_full(mat4f, 
	const GLfloat, const GLfloat, const GLfloat, const GLfloat, 
	const GLfloat, const GLfloat, const GLfloat, const GLfloat, 
	const GLfloat, const GLfloat, const GLfloat, const GLfloat,
	const GLfloat, const GLfloat, const GLfloat, const GLfloat);
void mat4f_diag(mat4f, const GLfloat);
//mat4f mat4f_from_arr(GLfloat[16]);
void vec4f_full(vec4f,
	const GLfloat, const GLfloat, const GLfloat, const GLfloat);
void vec4f_fill(vec4f, const GLfloat);
//vec4f vec4f_from_arr(GLfloat[4]);
void mat4f_mult(const mat4f, const mat4f, mat4f);
void mat4f_scale(mat4f, const GLfloat, mat4f);
void mat4f_add(const mat4f, const mat4f, mat4f);
void mat4f_transpose(mat4f, mat4f);
void mat4f_inverse(const mat4f, mat4f);
GLfloat mat4f_det(const mat4f);
void mat4f_print(mat4f);

#endif
