#include "../include/glad/glad.h"
#include "../include/GLFW/glfw3.h"
//#define STB_IMAGE_IMPLEMENTATION
//#include "stb_image.h"

#include "../include/glm/glm.hpp"
#include "../include/glm/gtc/matrix_transform.hpp"
#include "../include/glm/gtc/type_ptr.hpp"

#include "Shader.h"
#include "OBJLoader.h"

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);


// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

bool wireMode = false;

//camera terrain
glm::vec3 cameraPosT = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFrontT = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUpT = glm::vec3(0.0f, 1.0f, 0.0f);

//camera topdown
glm::vec3 cameraPosF = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraFrontF = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUpF = glm::vec3(0.0f, 1.0f, 0.0f);

//camera topdown
glm::vec3 cameraPosTD = glm::vec3(0.0f, 5.0f, 0.0f);
glm::vec3 cameraFrontTD = glm::vec3(0.0f, -1.0f, 0.0f);
glm::vec3 cameraUpTD = glm::vec3(0.0f, 0.0f, -1.0f);

bool firstMouse = true;
float yaw = -90.0f;	// look at front
float pitch = 0.0f;
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;
float fov = 45.0f;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

glm::vec3 terrain_vertices[] = {
	{ 10.0f,0.0f,10.0f },
	{ 10.0f,0.0f,-10.0f },
	{ -10.0f,0.0f,10.0f },
	{ -10.0f,0.0f,-10.0f },
};

enum {
	TERRAIN,
	FREE,
	TOPDOWN
};
int camera_index = 0;

int main() {

	//Initialize wire mode
	wireMode = true;
		
	std::cout << "Z. Cámara libre"
			<< "\nX. Cámara en terreno (no implementada aún)"
			<< "\nC. Cámara topdown (vista aérea fija)\n";	
		
	//Loading obj file
	OBJLoader loader;
	loader.setFile("../models/tank-green.obj");
	if (!loader.parseFile()) {
		std::cout << "Error opening file" << loader.getFile() << "\n";
		return -1;
	}

	//Getting vertices and indices vectors
	std::vector<glm::vec3> vertices = loader.getVertices();
	std::vector<unsigned int> indices = loader.getFaces();
		
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	//Mouse mode
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	// build and compile our shader zprogram
	// ------------------------------------
	Shader ourShader("vertex.vs", "fragment.fs");
	Shader terrShader("vertex.vs", "vertex_terrain.vs");

	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	ourShader.use();
	glm::mat4 projection = glm::perspective(glm::radians(75.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 500.0f);
	ourShader.setMat4("projection", projection); 

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{

		//wire mode
		if (wireMode) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
			
		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		// input
		// -----
		processInput(window);

		// render
		// ------
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer now!

		// activate shader
		ourShader.use();

		// camera/view transformation
		glm::mat4 view = glm::mat4(1.0f);

		switch (camera_index) {
			case TERRAIN: 
				view = glm::lookAt(cameraPosT, cameraPosT + cameraFrontT, cameraUpT);
				break;
			case FREE:
				view = glm::lookAt(cameraPosF, cameraPosF + cameraFrontF, cameraUpT);
				break;
			case TOPDOWN:
				view = glm::lookAt(cameraPosTD, cameraPosTD + cameraFrontTD, cameraUpTD);
				break;
			default:
				std::cout << "wut\n";
		}
		ourShader.setMat4("view", view);

		// render VAO
		glBindVertexArray(VAO);
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		ourShader.setMat4("model", model);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();

	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ----------------------------------deltaT-----------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) 
		camera_index = TERRAIN;
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
		camera_index = FREE;
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
		camera_index = TOPDOWN;
	

	float cameraSpeed = 2.5 * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		if (camera_index == TERRAIN )
			cameraPosT += cameraSpeed * cameraFrontT;
		//else if (camera_index == FREE) 
			//Free camera 
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		if (camera_index == TERRAIN)
			cameraPosT -= cameraSpeed * cameraFrontT;
		//else if (camera_index == FREE)
			//Free camera 
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		if (camera_index == TERRAIN)
			cameraPosT -= glm::normalize(glm::cross(cameraFrontT, cameraUpT)) * cameraSpeed;
		//else if (camera_index == FREE)
			//Free camera 
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		if (camera_index == TERRAIN)	
			cameraPosT += glm::normalize(glm::cross(cameraFrontT, cameraUpT)) * cameraSpeed;
		//else if (camera_index == FREE)
			//Free camera
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		std::cout << "x: " << xpos << std::endl;
		std::cout << "y: " << ypos << std::endl;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.1f; // change this value to your liking
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	// make sure that when pitch is out of bounds, screen doesn't get flipped
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFrontT = glm::normalize(front);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (fov >= 1.0f && fov <= 45.0f)
		fov -= yoffset;
	if (fov <= 1.0f)
		fov = 1.0f;
	if (fov >= 45.0f)
		fov = 45.0f;
}