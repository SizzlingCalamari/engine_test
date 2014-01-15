
#include <cstdint>
#include <iostream>

#include "World.h"

#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include <sys/stat.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

off_t GetFileSize(int fd)
{
	struct stat buf;
	int ret = fstat(fd, &buf);
	return ret == 0 ? buf.st_size : -1;
}

GLuint LoadShaders(const char *vertex_file, const char *fragment_file)
{
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	FILE *VertexShaderFile = fopen(vertex_file, "rb");
	FILE *FragmentShaderFile = fopen(fragment_file, "rb");

	if (!VertexShaderFile || !FragmentShaderFile)
	{
		if (VertexShaderFile) fclose(VertexShaderFile);
		if (FragmentShaderFile) fclose(FragmentShaderFile);
		return 0;
	}

	off_t vertex_size = GetFileSize(fileno(VertexShaderFile));
	off_t fragment_size = GetFileSize(fileno(FragmentShaderFile));

	char *VertexShaderCode = (char*)malloc(vertex_size + 1);
	char *FragmentShaderCode = (char*)malloc(fragment_size + 1);

	VertexShaderCode[vertex_size] = '\0';
	FragmentShaderCode[fragment_size] = '\0';
	
	fread(VertexShaderCode, 1, vertex_size, VertexShaderFile);
	fread(FragmentShaderCode, 1, fragment_size, FragmentShaderFile);

	fclose(VertexShaderFile);
	fclose(FragmentShaderFile);

	glShaderSource(VertexShaderID, 1, (const char**)&VertexShaderCode, nullptr);
	glCompileShader(VertexShaderID);

	glShaderSource(FragmentShaderID, 1, (const char**)&FragmentShaderCode, nullptr);
	glCompileShader(FragmentShaderID);

	GLint res = GL_FALSE;

	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &res);
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &res);

	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	glGetProgramiv(ProgramID, GL_LINK_STATUS, &res);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	free(FragmentShaderCode);
	free(VertexShaderCode);

	return ProgramID;
}

int main(int argc, char *argv[])
{
    std::cout << "Hello World!" << std::endl;

	glfwInit();

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindowHint(GLFW_RED_BITS, 0);
	glfwWindowHint(GLFW_GREEN_BITS, 0);
	glfwWindowHint(GLFW_BLUE_BITS, 0);
	glfwWindowHint(GLFW_ALPHA_BITS, 0);
	glfwWindowHint(GLFW_DEPTH_BITS, 32);
	glfwWindowHint(GLFW_STENCIL_BITS, 0);

	GLFWwindow* window = glfwCreateWindow(640, 480, "JORDAN", NULL, NULL);
	glfwMakeContextCurrent(window);

	glewExperimental = true;
	glewInit();

	glfwSetInputMode(window, GLFW_STICKY_KEYS, 1);
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	GLuint ProgramID = LoadShaders("../src/shaders/simplevertex.vs", "../src/shaders/simplefragment.fs");

	GLuint MatrixID = glGetUniformLocation(ProgramID, "MVP");
	
	glm::mat4 Projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
	glm::mat4 View = glm::lookAt(glm::vec3(4, 3, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	glm::mat4 Model = glm::mat4(1.0f);
	glm::mat4 MVP = Projection * View * Model;

	static const GLfloat vertex_data[] = 
	{
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
	};

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data), vertex_data, GL_STATIC_DRAW);

    World w;
    w.Initialize();

	const uint32_t SERVER_FRAME_DT = 10000;

	while (!glfwWindowShouldClose(window))
	{
		w.Update(SERVER_FRAME_DT);

		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(ProgramID);
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

		glDrawArrays(GL_TRIANGLES, 0, 3);
		glDisableVertexAttribArray(0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
    w.Shutdown();

	glfwDestroyWindow(window);
	glfwTerminate();

	glDeleteBuffers(1, &vertexbuffer);
	glDeleteVertexArrays(1, &VertexArrayID);
}
