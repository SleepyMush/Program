#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
#include <vector>
#include <array>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "/Program1/Program/Header Files/Shader.hpp"
#include "/Program1/Program/Header Files/Texture.hpp"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
unsigned int VBO, VAO, EBO;
std::vector<float> vertices;
void CreateQuad();

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

struct Quad
{
	/*Transform transform;*/
	std::array<Vertex, 6> vertices;
};

struct Entity
{
	Transform transform;
	size_t quadOffset;
};

Transform transform;
float LO = -1.0f;
float HI = 1.0f;

std::vector<Quad> quads;
void CreateQuad()
{
	Quad q;
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
	q.vertices.at(0) = v0;
	q.vertices.at(1) = v1;
	q.vertices.at(2) = v3;
	q.vertices.at(3) = v1;
	q.vertices.at(4) = v2;
	q.vertices.at(5) = v3;
}

static void add_vertices(std::vector<float>& v, float x, float y) {

	float vertices[] = {
		// positions         // colors
		0.5f + x, -0.5f + y, 0.0f,  1.0f, 0.0f, 0.0f,   // bottom right
		-0.5f + x, -0.5f + y, 0.0f,  0.0f, 1.0f, 0.0f,   // bottom left
		0.0f + x,  0.5f + y, 0.0f,  0.0f, 0.0f, 1.0f    // top 
	};

	v.insert(v.end(), std::begin(vertices), std::end(vertices));

	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(100 * 1024) * 1024, NULL, GL_DYNAMIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);
};

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
	Texture image("Resource Folder/Textures/download.png");
	stbi_set_flip_vertically_on_load(true);
	add_vertices(vertices, 0.0, 0.0);
	
	GLint location = glGetAttribLocation(shader.ID, "Position");
	glVertexAttribPointer(location, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(location);

	//for (int i = 0; i < 10; ++i) {
	//	float r3 = LO + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (HI - LO)));
	//	float r4 = LO + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (HI - LO)));
	//	add_vertices(vertices, r3, r4);
	//}

	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		shader.use();
		unsigned int transformLoc = glGetUniformLocation(shader.ID, "transform");
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform.to_mat4()));

		image.bind(0);
		//glBindVertexArray(VAO);
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(100 * 1024) * 1024, NULL, GL_STREAM_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(float), vertices.data());
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		shader.use();
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 6);

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


