#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 2) in vec3 normal;

out vec3 fragcolor;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{

	vec4 worldPosition = model * vec4(aPos.x, aPos.y , aPos.z, 1.0f);
	gl_Position = projection * view * worldPosition;
	fragcolor = vec3(normal.x, normal.y, normal.z) * 0.5;

}