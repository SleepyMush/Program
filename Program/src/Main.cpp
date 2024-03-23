#include <array>
#include <cmath>
#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "/Program1/Program/Header Files/Shader.hpp"
#include "/Program1/Program/Header Files/Texture.hpp"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
unsigned int VBO, VAO, EBO;
void CreateQuad();

float deltaTime = 0.0f;
float lastFrame = 0.0f;
float playerSpeed = 1.0f;
float fps;

std::vector<glm::mat4> transforms;

extern "C" {
	__declspec(dllexport) uint32_t NvOptimusEnablement = 1;
}

struct Transform
{
	glm::vec3 position = glm::vec3(0.0f);
	glm::vec3 rotation = glm::vec3(0.0f);
	glm::vec3 scale = glm::vec3(1.0f);
	glm::mat4 to_mat4() const
	{
		glm::mat4 m = glm::translate(glm::mat4(1.0f), position);
		m *= glm::mat4_cast(glm::quat(rotation));
		m = glm::scale(m, scale);
		return m;
	}
};

struct Vertex
{
	glm::vec2 position;
	glm::vec2 texCoords;
};

std::vector<Vertex> vertices;

//struct Entity
//{
//	Transform transform;
//	size_t quadOffset;
//};

Transform transform;
float LO = -1.0f;
float HI = 1.0f;

void CreateQuad()
{
	Vertex v0;
	Vertex v1;
	Vertex v2;
	Vertex v3;
	v0.position = glm::vec2(0.5f, 0.5f);
	v1.position = glm::vec2(0.5f, -0.5f);
	v2.position = glm::vec2(-0.5f, -0.5f);
	v3.position = glm::vec2(-0.5f, 0.5f);
	v0.texCoords = glm::vec2(1.0f, 0.0f);
	v1.texCoords = glm::vec2(1.0f, 1.0f);
	v2.texCoords = glm::vec2(0.0f, 1.0f);
	v3.texCoords = glm::vec2(0.0f, 0.0f);
	vertices.push_back(v0);
	vertices.push_back(v1);
	vertices.push_back(v3);
	vertices.push_back(v1);
	vertices.push_back(v2);
	vertices.push_back(v3);
}

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(1920, 1080, "Program", NULL, NULL); 
	if (window == NULL)
	{
		std::cout << "GLFW Context is incorrect" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	Shader shader("Resource Folder/Shader/Shader.vert","Resource Folder/Shader/Shader.frag");
	stbi_set_flip_vertically_on_load(false);
	Texture image("Resource Folder/Textures/download.png");

	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, 100 * 1024 * 1024, NULL, GL_DYNAMIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 1 * sizeof(Vertex),(void*)offsetof(Vertex, position));
	glEnableVertexAttribArray(0);

	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 1 * sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
	glEnableVertexAttribArray(1);

	GLuint SSBO;
	glGenBuffers(1, &SSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, 100 * 1024 * 1024, NULL, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, SSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); 

	
	for (int i = 0; i < 10; ++i) {
		Transform t;
		float r3 = LO + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (HI - LO)));
		float r4 = LO + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (HI - LO)));
		t.position = glm::vec3(r3, r4, 0.0f);
		transforms.push_back(t.to_mat4());
		CreateQuad();
	}

	//CreateQuad();

	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		shader.use();
		unsigned int transformLoc = glGetUniformLocation(shader.ID, "transform");
		glUniformMatrix4fv(transformLoc, 2, GL_FALSE, glm::value_ptr(transform.to_mat4()));

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO);
		glBufferData(GL_SHADER_STORAGE_BUFFER, 100 * 1024 * 1024, NULL, GL_DYNAMIC_DRAW);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, transforms.size() * sizeof(glm::mat4), transforms.data());
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		fps = 1.0f / deltaTime;

		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		{
			transform.position.x += playerSpeed * deltaTime;
		}

		image.bind(0);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, 100 * 1024 * 1024, NULL, GL_DYNAMIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(Vertex), vertices.data());
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		shader.use();
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, vertices.size());

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	image.cleanUp();
	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}


//static void add_vertices(std::vector<float>& v, float x, float y) {
//
//	float vertices[] = {
//		// positions         // colors
//		0.5f + x, -0.5f + y, 0.0f,  1.0f, 0.0f, 0.0f,   // bottom right
//		-0.5f + x, -0.5f + y, 0.0f,  0.0f, 1.0f, 0.0f,   // bottom left
//		0.0f + x,  0.5f + y, 0.0f,  0.0f, 0.0f, 1.0f    // top 
//	};
//
//	v.insert(v.end(), std::begin(vertices), std::end(vertices));
//
//	glGenBuffers(1, &VBO);
//	glGenVertexArrays(1, &VAO);
//	glBindVertexArray(VAO);
//
//	glBindBuffer(GL_ARRAY_BUFFER, VBO);
//	glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(100 * 1024) * 1024, NULL, GL_DYNAMIC_DRAW);
//
//	// position attribute
//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
//	glEnableVertexAttribArray(0);
//
//	// color attribute
//	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
//	glEnableVertexAttribArray(1);
//	glBindVertexArray(0);
//};