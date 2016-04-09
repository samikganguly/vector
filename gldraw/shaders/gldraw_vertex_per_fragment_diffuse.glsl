#version 400

in vec3 gldraw_vert;
in vec3 gldraw_norm;

uniform mat4 gldraw_view;
uniform mat4 gldraw_projection;
uniform mat4 gldraw_model;

out vec3 position;
out vec3 normal;

void main() {
	mat3 normalMatrix = mat3(transpose(inverse(gldraw_view * gldraw_model)));
	normal = normalize(normalMatrix * gldraw_norm);
	vec4 pos = gldraw_projection * (gldraw_view * gldraw_model) * vec4(gldraw_vert, 1);
	position = vec3(pos);
	gl_Position = pos;
}
