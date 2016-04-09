#include <string.h>
#include <strings.h>
#include <GL/glew.h>
#include <gtk/gtk.h>
#include "matrix.h"

void mat4f_full(mat4f m, 
	const GLfloat a00, const GLfloat a01, const GLfloat a02, const GLfloat a03, 
	const GLfloat a10, const GLfloat a11, const GLfloat a12, const GLfloat a13, 
	const GLfloat a20, const GLfloat a21, const GLfloat a22, const GLfloat a23,
	const GLfloat a30, const GLfloat a31, const GLfloat a32, const GLfloat a33) {
	m[0][0] = a00; m[0][1] = a01; m[0][2] = a02; m[0][3] = a03; m[1][0] = a10;
	m[1][1] = a11; m[1][2] = a12; m[1][3] = a13; m[2][0] = a20; m[2][1] = a21;
	m[2][2] = a22; m[2][3] = a23; m[3][0] = a30; m[3][1] = a31; m[3][2] = a32;
	m[3][3] = a33;
}

void mat4f_diag(mat4f m, const GLfloat a) {
	bzero(m, 16 * sizeof(GLfloat));
	m[0][0] = m[1][1] = m[2][2] = m[3][3] = a;
}

/*mat4f mat4f_from_arr(GLfloat arr[16]) {
	return (mat4f)arr;
}*/

void vec4f_full(vec4f v,
	const GLfloat a0, const GLfloat a1, const GLfloat a2, const GLfloat a3) {
	v[0] = a0; v[1] = a1; v[2] = a2; v[3] = a3;
}

void vec4f_fill(vec4f v, const GLfloat a) {
	memset(v, a, 4 * sizeof(GLfloat));
}

/*vec4f vec4f_from_arr(GLfloat arr[4]) {
	return (vec4f)arr;
}*/

void mat4f_mult(const mat4f m1, const mat4f m2, mat4f result) {
	for(int i = 0; i < 4; ++i) {
		for(int j = 0; j < 4; ++j) {
			for(int k = 0; k < 4; ++k)
				result[i][j] += m1[i][k] * m2[k][i];
		}
	}
}

void mat4f_scale(mat4f m, const GLfloat scale, mat4f result) {
	for(int i = 0; i < 4; ++i) {
		for(int j = 0; j < 4; ++j) {
			if(result)
				result[i][j] = m[i][j] * scale;
			else
				m[i][j] *= scale;
		}
	}
}

void mat4f_add(const mat4f m1, const mat4f m2, mat4f result) {
	for(int i = 0; i < 4; ++i) {
		for(int j = 0; j < 4; ++j) {
			result[i][j] = m1[i][j] + m2[i][j];
		}
	}
}

void mat4f_transpose(mat4f m, mat4f result) {
	if(result) {
		for(int i = 0; i < 4; ++i) {
			for(int j = 0; j < 4; ++j) {
				result[i][j] = m[j][i];
			}
		}
	} else {
		GLfloat rest[3];
		rest[0] = m[0][1]; rest[1] = m[0][2]; rest[2] = m[0][3];
		m[0][1] = m[1][0]; m[0][2] = m[2][0]; m[0][3] = m[3][0];
		m[1][0] = rest[0]; m[2][0] = rest[1]; m[3][0] = rest[2];
		
		rest[0] = m[1][2]; rest[1] = m[1][3];
		m[1][2] = m[2][1]; m[1][3] = m[3][1];
		m[2][1] = rest[0]; m[3][1] = rest[1];

		rest[0] = m[2][3];
		m[2][3] = m[3][2];
		m[3][2] = rest[0];
	}
}

void mat4f_inverse(const mat4f m, mat4f result) {
	mat4f L, U;
	vec4f d, b;
	mat4f_diag(L, 0);
	mat4f_diag(U, 1);
	mat4f_diag(result, 0);
	for(int i = 0; i < 4; ++i) {
		L[i][0] = m[i][0];
		if(i) U[0][i] = m[0][i] / L[0][0];
	}
	for(int j = 1; j < 3; ++j) {
		for(int i = j; i < 4; ++i) {
			GLfloat tmp = 0;
			for(int k = 0; k < j; ++k)
				tmp += L[i][k] * U[k][j];
			L[i][j] = m[i][j] - tmp;
			tmp = 0;
			if(i > j) {
				for(int k = 0; k < j; ++k)
					tmp += L[j][k] * U[k][i];
				U[j][i] = (m[j][i] - tmp) / L[j][j];
			}
		}
	}
	L[3][3] = m[3][3];
	for(int k = 0; k < 3; ++k) {
		L[3][3] -= L[3][k] * U[k][3];
	}
	
	for(int i = 0; i < 4; ++i) {
		vec4f_fill(b, 0);
		b[i] = 1;
		d[0] = b[0] / L[0][0];
		for(int j = 1; j < 4; ++j) {
			GLfloat tmp = 0;
			for(int k = 0; k < j; ++k)
				tmp += L[j][k] * d[k];
			d[j] = (b[j] - tmp) / L[j][j];
		}
		result[3][i] = d[3];
		for(int j = 2; j > -1; --j) {
			GLfloat tmp = 0;
			for(int k = j + 1; k < 4; ++k)
				tmp += U[j][k] * result[k][i];
			result[j][i] = d[j] - tmp;
		}
	}
}

GLfloat mat4f_det(const mat4f m) {
	GLfloat il = m[2][2] * m[3][3] - m[2][3] * m[3][2],
	        fk = m[2][1] * m[3][3] - m[2][3] * m[3][1],
	        eh = m[2][1] * m[3][2] - m[2][2] * m[3][0],
	        cj = m[2][0] * m[3][3] - m[2][3] * m[3][0],
	        bg = m[2][0] * m[3][2] - m[2][2] * m[3][0],
	        ad = m[2][0] * m[3][1] - m[2][1] * m[3][0];
	return m[0][0] * (m[1][1] * il - m[1][2] * fk + m[1][3] * eh)
	     - m[0][1] * (m[1][0] * il - m[1][2] * cj + m[1][3] * bg)
	     + m[0][2] * (m[1][0] * fk - m[1][1] * cj + m[1][3] * ad)
	     - m[0][3] * (m[1][0] * eh - m[1][1] * bg + m[1][2] * ad);
}

void mat4f_print(mat4f m) {
	for(int i = 0; i < 4; ++i) {
		for(int j = 0; j < 4; ++j) {
			g_print("%+3.5f ", m[i][j]);
		}
		g_print("\n");
	}
	g_print("\n");
}
