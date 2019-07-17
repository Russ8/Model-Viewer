#include <vector>
#include <string>
#include <glm/glm.hpp>
#ifndef SHAPE_H
#define SHAPE_H

class Shape {
public:
	Shape() {}
	int verts;
	int vaoID;
	int texture;
	float shininess;
	glm::vec3 ambient;
	glm::vec3 specular;
	glm::vec3 diffuse;

};

#endif