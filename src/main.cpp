#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <string>
#include <vector>
#include "Camera.h"
#include "Shape.h"
#include "Shader.h"
#include "Light.h"
#include "stb_image.h"
#include "tiny_obj_loader.h"

//callback function prototypes
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow *window);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

// window settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

//create camera
Camera * camera = new Camera(glm::vec3(0.0f, 0.2f, 0.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;

// timing
float deltaTime = 0.0f;	
float lastFrame = 0.0f;
int timer = 0;

//use only textures
bool useTextures;

//mouse settings
bool firstMouse = true;
float mouseX = 0;
float mouseY = 0;

//mode settings
int debug_mode = 0;
int usage_mode = 0;
int light_mode = 0;
bool allow_pan = false;
float Zoom = 1;
float fov = 65.0;
float angle = 0;

Light * light;
Light * light1;
Light * light2;
Light * light3;

int loadTexture(std::string file) {
	unsigned int texture1;
	// texture 1
	// ---------
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1); //make sure glfw is already set up
											// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(false); // tell stb_image.h to flip loaded texture's on the y-axis.
	unsigned char *data = stbi_load(file.c_str(), &width, &height, &nrChannels, 0); //contains pixel data
	for (int i = 0; i < width * height; i++) {
		//std::cout << data[i] - '0' << " ";
	}
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Using default texture" << std::endl;

		texture1 = loadTexture("blank.png");
	}

	stbi_image_free(data);

	return texture1;
}

int main(int argc, char **argv) {

	if(argc<=1) {
		std::cout <<"incorrect parameters" << std::endl;
		return 0;
	} 
	std::string file_name = argv[1];
	//glfw settings
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		exit(1);
	}

	useTextures = true;

	//creating shaders

	Shader * debug_inspect = new Shader("debug_inspect.vert", "debug_inspect.frag");
	Shader * light_shading = new Shader("light-shading.vert", "light-shading.frag");
	
	light1 = new Light(glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(1.0, 0.5, 0.5), glm::vec3(0.5, 0.5, 1.0), 1); //directional red light
	light2 = new Light(glm::vec3(0.0, 4.0, 0.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.5, 1.0, 0.5), glm::vec3(0.5, 1.0, 0.5), 0); //blue cam light
	light3 = new Light(glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(1.0, 1.0, 0.0), glm::vec3(1.0, 1.0, 0.5), 0); //yellow rotating object

	light = light1; //currently used light

	std::string inputfile = file_name; 

	//-----------------  load obj file --------------------
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string err;
	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, inputfile.c_str());

	if (!err.empty()) { // `err` may contain warning message.
		std::cerr << err << std::endl;
	}

	if (!ret) {
		exit(1);
	}

	//get min, max values
	float max_width = 0.0f;
	float min_x = 99999999.0;
	float max_x = -9999999.0;
	float min_y = 99999999.0;
	float max_y = -99999999.0;
	float min_z = 9999999.0;
	float max_z = -9999999.0;

	for (int i = 0; i < attrib.vertices.size(); i+=3) {
		float wx = attrib.vertices[i];
		if (wx < min_x) {
			min_x = wx;
		}
		if (wx > max_x) {
			max_x = wx;
		}
		wx = attrib.vertices[i+1];
		if (wx < min_y) {
			min_y = wx;
		}
		if (wx > max_y) {
			max_y = wx;
		}
		wx = attrib.vertices[i+2];
		if (wx < min_z) {
			min_z = wx;
		}
		if (wx > max_z) {
			max_z = wx;
		}
	}

	std::cout << "max x: " << max_x << " min x: " << min_x << " max y: " << max_y << " min y: " << min_y << " max z: " << max_z << " min z: " << min_z << std::endl;

	//calculate scale factor
	float max_width_x = abs(max_x - min_x);
	float max_width_y = abs(max_y - min_y);
	float max_width_z = abs(max_z - min_z);

	if (max_width_x > max_width) {
		max_width = max_width_x;
	}
	if (max_width_y > max_width) {
		max_width = max_width_y;
	}
	if (max_width_z > max_width) {
		max_width = max_width_z;
	}

	Zoom = 2.0f / max_width;

	std::cout << "scaling object by factor of: " << Zoom << std::endl;
	
	//attempt to load texture
	int obj_texture;
	if (materials.size() > 0  ) {
		obj_texture = loadTexture(materials.at(0).diffuse_texname);
	}

	//create indices vector
	std::vector<int> indicies;
	for (int i = 0; i < shapes[0].mesh.indices.size(); i++) {
		indicies.push_back(shapes[0].mesh.indices[i].vertex_index);
	}

	//default material values
	glm::vec3 m_ambient = glm::vec3(0.0, 0.0, 0.0);
	glm::vec3 m_diffuse = glm::vec3(1.0, 0.0, 0.0);
	glm::vec3 m_specular = glm::vec3(1.0, 1.0, 1.0);
	float shininess = 1.0;


	std::vector<Shape*> shapeList;

	// Loop over shapes
	for (size_t s = 0; s < shapes.size(); s++) {
		Shape * shape = new Shape();
		shape->texture = loadTexture(materials.at(shapes[s].mesh.material_ids[0]).diffuse_texname);
		std::vector<float> shapeVertices;
		std::vector<float> shapeNormals;
		std::vector<float> shapeTextures;
		// Loop over faces(polygon)
		size_t index_offset = 0;
		for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
			int fv = shapes[s].mesh.num_face_vertices[f];

			// Loop over vertices in the face.
			for (size_t v = 0; v < fv; v++) {
				// access to vertex
				tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
				shapeVertices.push_back( attrib.vertices[3 * idx.vertex_index + 0]);
				shapeVertices.push_back(attrib.vertices[3 * idx.vertex_index + 1]);
				shapeVertices.push_back(attrib.vertices[3 * idx.vertex_index + 2]);
				if (attrib.normals.size() > 0) {
					shapeNormals.push_back(attrib.normals[3 * idx.normal_index + 0]);
					shapeNormals.push_back(attrib.normals[3 * idx.normal_index + 1]);
					shapeNormals.push_back(attrib.normals[3 * idx.normal_index + 2]);
				}
				else {
					shapeNormals.push_back(0.0);
					shapeNormals.push_back(0.0);
					shapeNormals.push_back(0.0);
				}
				if (attrib.texcoords.size() > 0) {
					shapeTextures.push_back(attrib.texcoords[2 * idx.texcoord_index + 0]);
					shapeTextures.push_back(attrib.texcoords[2 * idx.texcoord_index + 1]);
				}
				// Optional: vertex colors
				// tinyobj::real_t red = attrib.colors[3*idx.vertex_index+0];
				// tinyobj::real_t green = attrib.colors[3*idx.vertex_index+1];
				// tinyobj::real_t blue = attrib.colors[3*idx.vertex_index+2];
			}
			index_offset += fv;

			// per-face material
			shapes[s].mesh.material_ids[f];
		}

		//copy data to GPU
		unsigned int VBO, VAO, EBO, VBOt, VBOn;
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &VBOt);
		glGenBuffers(1, &VBOn);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, shapeVertices.size() * sizeof(float), shapeVertices.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		if (attrib.texcoords.size() > 0) {
			glBindBuffer(GL_ARRAY_BUFFER, VBOt);
			glBufferData(GL_ARRAY_BUFFER, shapeTextures.size() * sizeof(float), shapeTextures.data(), GL_STATIC_DRAW);

			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*) 0);
			glEnableVertexAttribArray(1);
		}


		glBindBuffer(GL_ARRAY_BUFFER, VBOn);
		glBufferData(GL_ARRAY_BUFFER, shapeNormals.size() * sizeof(float), shapeNormals.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);
		glEnableVertexAttribArray(2);

		glBindBuffer(GL_ARRAY_BUFFER, 2);
		
		shape->verts = shapeVertices.size();
		shape->vaoID = VAO;

		//load material values
		if (materials.size() > 0) {
			shape->ambient = glm::vec3(materials.at(shapes[s].mesh.material_ids[0]).ambient[0] , materials.at(shapes[s].mesh.material_ids[0]).ambient[1], materials.at(shapes[s].mesh.material_ids[0]).ambient[2]);
			shape->diffuse = glm::vec3(materials.at(shapes[s].mesh.material_ids[0]).diffuse[0], materials.at(shapes[s].mesh.material_ids[0]).diffuse[1], materials.at(shapes[s].mesh.material_ids[0]).diffuse[2]);
			shape->specular = glm::vec3(materials.at(shapes[s].mesh.material_ids[0]).specular[0], materials.at(shapes[s].mesh.material_ids[0]).specular[1], materials.at(shapes[s].mesh.material_ids[0]).specular[2]);
			shape->shininess = materials.at(shapes[s].mesh.material_ids[0]).shininess / 100;
		}


		shapeList.push_back(shape);
	}


	

	//enable depth test
	glEnable(GL_DEPTH_TEST);

	//main loop
	while (!glfwWindowShouldClose(window))
	{
		if (timer != 0) timer--;
		
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		//setup light position
		if (light_mode == 1) {
			light->position = camera->Position + glm::vec3(0.0, 1.0, 0.0);
		}
		else if (light_mode == 2) {
			angle+=0.1;
			light->position = glm::vec3(10 * cos(angle), 0.0, 10 * sin(angle));
		}


		processInput(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//create projection, view matrices
		glm::mat4 projection;
		projection = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		
		glm::mat4 view; 
		view = camera->GetViewMatrix();

		glm::mat4 model;
		model = glm::translate(model, glm::vec3(0.0,  0.0, -1.5 * max_width * Zoom));
		model = glm::scale(model, glm::vec3(Zoom, Zoom, Zoom));

		if (usage_mode == 1) { //if in debug mode
			//setup uniforms for debug shader
			debug_inspect->use();
			debug_inspect->setMat4("projection", projection);
			debug_inspect->setMat4("view", view);
			debug_inspect->setInt("mode", debug_mode);
			debug_inspect->setMat4("model", model);
		}
		else if (usage_mode == 0) { //if in light shading mode
			//setup uniforms for light shading shader
			light_shading->use();
			light_shading->setVec3("l_ambient", light->ambientColor);
			light_shading->setVec3("l_diffuse", light->diffuseColor);
			light_shading->setVec3("l_specular", light->specularColor);
			light_shading->setVec3("lightPosition", light->position);
			light_shading->setFloat("shineDamper", 10.0);
			light_shading->setInt("lightType", light->type);
			light_shading->setInt("useTextures", useTextures);
			light_shading->setMat4("projection", projection); 
			light_shading->setMat4("view", view);
			light_shading->setInt("mode", debug_mode);
			light_shading->setMat4("model", model);

			

			//if in light mode 3, use only textures, no lights
			if (light_mode == 3) {
				light_shading->setInt("useTextures", 1);
			}
			else {
				light_shading->setInt("useTextures", 0);
			}
			
		}

		for (int i = 0; i < shapeList.size(); i++) {
			if (usage_mode == 1) {
				debug_inspect->setVec3("material_color", m_diffuse);
			}
			else if (usage_mode == 0) {
				light_shading->setFloat("reflectivity", shapeList.at(i)->shininess);
				light_shading->setVec3("m_ambient", shapeList.at(i)->ambient);
				light_shading->setVec3("m_diffuse", shapeList.at(i)->diffuse);
				light_shading->setVec3("m_specular", shapeList.at(i)->specular);
				light_shading->setVec3("material_color", shapeList.at(i)->diffuse);
			}

			//setup texture
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, shapeList.at(i)->texture);

			//draw vao
			glBindVertexArray(shapeList.at(i)->vaoID);
			glDrawArrays(GL_TRIANGLES, 0, shapeList.at(i)->verts);
		}
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) {
		//the timer allows ease of use
		if (timer == 0) {
			if (usage_mode == 1) {
				debug_mode++;

				if (debug_mode >= 3) {
					debug_mode = 0;
				}

				timer = 10;
			
				//debug mode 0 uses only wireframes
				if (debug_mode == 0) {
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

				}
				else {
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				}
			}
		}
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		if (timer == 0) {
			usage_mode++;

			if (usage_mode >= 2) {
				usage_mode = 0;
			}

			timer = 10;

			//in the case we're going back to debug mode, make sure wireframe mode is on if need be
			if ( (debug_mode == 0) && (usage_mode == 1) ){
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

			}
			else {
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			}
		}
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		if (timer == 0) {
			if (usage_mode == 0) {
				light_mode++;

				if (light_mode >= 4) {
					light_mode = 0;
				}

				timer = 10;

				//simply swap the current light (light) to the required light source
				if (light_mode == 0) {
					light = light1;
				}
				else if (light_mode == 1) {
					light = light2;
				}
				else if (light_mode == 2) {
					light = light3;
				}
			}
		}
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	mouseX = xpos;
	mouseY = ypos;

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;
	//caluclate zoom and apply by altering field of view used in projection matrix
	camera->ProcessMouseMovement(xpos, ypos, xoffset, yoffset);
	if(allow_pan  == true) 
		fov -= 0.05f * yoffset;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	//allow pan allows mouse movement to zoom
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		allow_pan = true;
	
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
		allow_pan = false;
	}	
}
