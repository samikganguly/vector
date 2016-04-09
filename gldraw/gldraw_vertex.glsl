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
	vec3 normEye = normalize(-eye.xyz);
	vec3 reflectedLight = reflect(-lightVec, normal);
	float sdotn = max(dot(lightVec, normal), 0);
	outColor = lightColor * surfaceColor * sdotn;
	//uncomment for toon shader
	/*float stepSize = 4.0f;
	float band = float(int(outColor.r * stepSize)) / stepSize;
	outColor = vec3(band, band, band);*/
	if(sdotn > 0)
		outColor += pow(max(dot(reflectedLight, normEye), 0), 50);
	gl_Position = gldraw_projection * (gldraw_view * gldraw_model) * vec4(gldraw_vert, 1);
}
