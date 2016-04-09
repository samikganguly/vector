#version 400

in vec3 gldraw_vert;
in vec3 gldraw_norm;

uniform mat4 gldraw_lamp;
uniform mat4 gldraw_view;
uniform mat4 gldraw_projection;
uniform mat4 gldraw_model;

out vec3 outColor;

void main() {
	vec3 lightColor = vec3(1, 1, 1); //white light
	vec3 surfaceColor = vec3(1, 1, 1); //diffuse reflection color white
	mat3 normalMatrix = mat3(transpose(inverse(gldraw_view * gldraw_model)));
	vec4 lightPos = gldraw_lamp * vec4(0, 0, 0, 1);
	vec3 normal = normalize(normalMatrix * gldraw_norm);
	vec4 eye = (gldraw_view * gldraw_model) * vec4(gldraw_vert, 1);
	vec3 lightVec = normalize(vec3(lightPos - eye));
	outColor = lightColor * surfaceColor * max(dot(lightVec, normal), 0);
	gl_Position = gldraw_projection * (gldraw_view * gldraw_model) * vec4(gldraw_vert, 1);
}
