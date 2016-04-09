#version 400

in vec3 position;
in vec3 normal;

uniform mat4 gldraw_lamp;

out vec4 FragColor;

void main() {
	vec3 lightColor = vec3(1, 1, 1); //white light
	vec3 surfaceColor = vec3(1, 1, 1); //diffuse reflection color white
	vec4 lightPos = gldraw_lamp * vec4(0, 0, 0, 1);
	vec3 lightVec = normalize(vec3(lightPos) - position);
	vec3 normEye = normalize(-position);
	vec3 reflectedLight = reflect(-lightVec, normal);
	float sdotn = max(dot(lightVec, normal), 0);
	vec3 outColor = lightColor * surfaceColor * sdotn;
	if(sdotn > 0)
		outColor += pow(max(dot(reflectedLight, normEye), 0), 50);
	FragColor = vec4(outColor, 1);
}
