#version 330 core

out vec4 out_color;
in vec3 fragcolor;
uniform int mode;
uniform vec3 material_color;

void main(void) {
	if(mode == 0) {
		out_color = vec4(-1.0, 0.5, 0.5, 1.0);
	} else if(mode == 1) {
		out_color = vec4(fragcolor, 1.0);
	} else if(mode == 2) {
		out_color = vec4(material_color, 1.0);
	}
}
