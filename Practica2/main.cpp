#include <glad/glad.h>
#include <GLFW/glfw3.h>
//#define STB_IMAGE_IMPLEMENTATION
//#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "OBJModel.h"

#include <iostream>

//Callback prototypes
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// Window settings
const unsigned int screenWidth = 800;
const unsigned int screenHeight = 600;

// Camera vectors
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

// Timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

// Wire mode setting
bool wireMode = false;

int main() {
	
	//OBJ file variables
	std::string file_name;
	OBJModel loader;

	while (file_name != "exit") {

		//Initialize wire mode
		wireMode = false;

		//Initialize camera
		cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
		cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
		cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

		//OBJ file input
		std::cout << "Ingrese \"exit\" para salir. \nIngrese el nombre del modelo a cargar (incluir extension .obj): ";
		std::cin >> file_name;

		//Break main loop
		if (file_name == "exit") {
			break;
		}

		//Validating existing OBJ file
		if (!loader.setFile("../models/" + file_name)) {
			continue;
		}

		// Initialize and configure GLFW
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		// Create GLFW Window
		GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "Model Viewer", NULL, NULL);
		if (window == nullptr) {
			std::cout << "Failed to create GLFW window" << std::endl;
			glfwTerminate();
			return -1;
		}
		glfwMakeContextCurrent(window);
		glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

		// Load OpenGL function pointers (GLAD)
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
			std::cout << "Failed to initialize GLAD" << std::endl;
			return -2;
		}

		// Configure global OpenGL state
		glEnable(GL_DEPTH_TEST);

		// Build and compile shaders
		Shader ourShader("vertex.vs", "fragment.fs");

		// Load OBJ file
		if (!loader.load()) {
			continue;
		}

		// Projection matrix
		ourShader.use();
		glm::mat4 projection = glm::perspective(glm::radians(75.0f), (float)screenWidth / (float)screenHeight, 0.1f, 500.0f);
		ourShader.setMat4("projection", projection);

		// Render loop
		while (!glfwWindowShouldClose(window))
		{

			//Wire mode enable/disable
			if (wireMode) {
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			}
			else {
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			}

			// Time logic
			float currentFrame = glfwGetTime();
			deltaTime = currentFrame - lastFrame;
			lastFrame = currentFrame;
			
			// Input
			processInput(window);

			// Render
			glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// Enable shader
			ourShader.use();

			// Camera transformation (View matrix)
			glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
			ourShader.setMat4("view", view);

			// Render OBJ model (Model matrix)
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
			ourShader.setMat4("model", model);
			loader.render();

			// Swap buffers and poll events (I/O)
			glfwSwapBuffers(window);
			glfwPollEvents();
		}

		// Clear GLFW resources
		glfwTerminate();

	}

	return 0;
}

// Keyboard processing
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
		wireMode = false;
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
		wireMode = true;

	float cameraSpeed = 2.5 * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraUp;
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraUp;
}

// Resize callback
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	//Adjust viewport
	glViewport(0, 0, width, height);
}