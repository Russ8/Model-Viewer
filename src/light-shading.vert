#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 tex;
layout (location = 2) in vec3 normal;

out vec3 normals;
out vec3 toLightVector;
out vec3 toCameraVector;
out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 lightPosition;
uniform int lightType;

void main()
{

	vec4 worldPosition = model * vec4(aPos.x, aPos.y , aPos.z, 1.0f);
	gl_Position = projection * view * worldPosition;

	normals = (model * vec4(normal, 0.0)).xyz;

	if(lightType == 0) {
		toLightVector = lightPosition - worldPosition.xyz;
	} else if (lightType == 1) {
		toLightVector = lightPosition;
	}

	TexCoord = tex;
	
	toCameraVector = (inverse(view) * vec4(0.0, 0.0, 0.0, 1.0)).xyz - worldPosition.xyz;
}