#include "../lib/GLAD/glad.h"
#include <GLFW/glfw3.h>
#include "../lib/shader.h"
#include <iostream>
#include <math.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stdlib.h>
#include <time.h>
#include <fstream>
#include <regex.h>
#define STB_IMAGE_IMPLEMENTATION
#include "../lib/stb_image.h"
#include "cube.h"
#include "camera.h"
#include "dataStructure.h"

using namespace glm;
#define MAX_MATCHES 3 //The maximum number of matches allowed in a single string
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void readFile(string path, DataStructure& ds);
bool match(regex_t *pexp, string sz, regmatch_t matches[]);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;

// camera
glm::vec3 cameraPos = glm::vec3(48.5f, 50.3f, 42.0f);
glm::vec3 cameraFront = glm::vec3(-0.50f, -0.77f, -0.40f);
glm::vec3 cameraUp = glm::vec3(-0.66f, 0.63f, -0.40f);

glm::vec3 lightPos = glm::vec3(0.0f, 0.0f, 10.0f);
glm::vec3 lightDirection = glm::vec3(0.0f, 0.01f, -1.0f);
glm::vec3 lightUp = glm::vec3(0.09f, 1.0f, -0.01);
glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 0.0f);
glm::vec3 cubeColor = glm::vec3(0.8f, 0.8f, 0.8f);

bool firstMouse = true;
bool moved = false;
bool pressed1 = false;
bool pressed2 = false;
glm::vec3 spotRotation = glm::vec3(0.975f, 1.0f, 1.2f);
Camera cam(cameraPos, cameraUp, cameraFront);
Camera light(lightPos, lightDirection, lightUp);
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;
float fov = 45.0f;
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;
float currentFrame;

int main()
{
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
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	light.setYaw(-90.0f);
	light.setPitch(-1.0f);

	//////////////////////////////DATE///////////////////////////////////////////////
	DataStructure topography;
	DataStructure lavaThickess;
	DataStructure lavaTemp;
	DataStructure example;
	DataStructure dataSource;
//	readFile("Data/altitudes.dat", dataSource);
//	readFile("Data/lava.dat", dataSource);
//	readFile("Data/temperature.dat", dataSource);
	readFile("Data/DEM_test.dat", dataSource);
//	readFile("Data/DEM_Albano.asc", dataSource);
	//////////////////////////////DATE///////////////////////////////////////////////
	////////////////////////////floor///////////////////////////////////////////////
	int numberCells = dataSource.getRows() * dataSource.getCols();
	int cont = 0;
	int nCols = dataSource.getCols();
	float vertex[dataSource.getRows()][dataSource.numberElementRow()];
	unsigned int VBOfloor[dataSource.getRows()];
	unsigned int VAOfloor[dataSource.getRows()];
	for (int i = 0; i < dataSource.getRows(); i++)
	{
		for (int j = 0; j < dataSource.getCols(); j++)
			dataSource.generatevertex(vertex[i], i, j, cont);
		glGenVertexArrays(1, &VAOfloor[i]);
		glGenBuffers(1, &VBOfloor[i]);
		glBindBuffer(GL_ARRAY_BUFFER, VBOfloor[i]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertex[i]), vertex[i], GL_STATIC_DRAW);
		glBindVertexArray(VAOfloor[i]);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) 0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) (3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		cont = 0;
	}

	////////////////////////////floor///////////////////////////////////////////////
	Shader lightShader("src/lightShader.vs", "src/lightShader.fs");
	Shader objShader("src/objects.vs", "src/objects.fs");
	////////////////////////////SHDAERS//////////////////////////////
//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_ALWAYS);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	while (!glfwWindowShouldClose(window))
	{
		currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		processInput(window);
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		objShader.use();
		//glm::mat4 projectionProspective = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 100.0f);
		glm::mat4 projectionL = glm::perspective(glm::radians(fov), (float) SCR_WIDTH / (float) SCR_HEIGHT, 0.1f,
				100.0f);
		glm::mat4 viewL = cam.GetViewMatrix();
		objShader.setMat4("projection", projectionL);
		objShader.setMat4("view", viewL);
		glm::mat4 modelL;
		objShader.setMat4("model", modelL);
		for (int i = 0; i < dataSource.getRows(); i++)
		{
			glBindVertexArray(VAOfloor[i]);
			float angleL = 0;
			modelL = glm::rotate(modelL, glm::radians(angleL), glm::vec3(1.0f, 0.3f, 0.5f));
			objShader.setMat4("model", modelL);
			glDrawArrays(GL_TRIANGLES, 0, sizeof(vertex[i]));
		}
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	for (int i = 0; i < dataSource.getRows(); i++)
	{
		glDeleteVertexArrays(1, &VAOfloor[i]);
		glDeleteBuffers(1, &VBOfloor[i]);
	}


	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}
void processInput(GLFWwindow *window)
{

	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
	{
		cam.resetVisual();
		moved = true;
	}
	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
	{
		light.resetVisual();
		moved = true;
	}

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
	{
		cout << "lightInfo" << endl;
		cout << "yaw:" << light.Yaw << "  " << "Pitch:" << light.Pitch << endl;
		cout << light.Position.x << "/" << light.Position.y << "/" << light.Position.z << endl;
		cout << light.Front.x << "/" << light.Front.y << "/" << light.Front.z << endl;
		cout << light.Up.x << "/" << light.Up.y << "/" << light.Up.z << endl;
		cout << "camInfo" << endl;
		cout << "yaw:" << cam.Yaw << "  " << "Pitch:" << cam.Pitch << endl;
		cout << cam.Position.x << "/" << cam.Position.y << "/" << cam.Position.z << endl;
		cout << cam.Front.x << "/" << cam.Front.y << "/" << cam.Front.z << endl;
		cout << cam.Up.x << "/" << cam.Up.y << "/" << cam.Up.z << endl;
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		cam.ProcessKeyboard(FORWARD, deltaTime);
		moved = true;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		cam.ProcessKeyboard(BACKWARD, deltaTime);
		moved = true;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		cam.ProcessKeyboard(LEFT, deltaTime);
		moved = true;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		cam.ProcessKeyboard(RIGHT, deltaTime);
		moved = true;
	}
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		light.ProcessKeyboard(FORWARD, deltaTime);
		moved = true;
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		light.ProcessKeyboard(BACKWARD, deltaTime);
		moved = true;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		light.ProcessKeyboard(LEFT, deltaTime);
		moved = true;
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		light.ProcessKeyboard(RIGHT, deltaTime);
		moved = true;
	}

}
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS)
	{
		pressed1 = true;

		if (firstMouse || moved || pressed2)
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
			moved = false;
		}

		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos;

		lastX = xpos;
		lastY = ypos;

		cam.ProcessMouseMovement(xoffset, yoffset);
		pressed2 = false;
	}
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS)
	{
		pressed2 = true;
		if (firstMouse || moved || pressed1)
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
			moved = false;
		}

		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos;

		lastX = xpos;
		lastY = ypos;

		light.ProcessMouseMovement(xoffset, yoffset);
		pressed1 = false;
	}
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	cam.ProcessMouseScroll(yoffset);
}
void readFile(string path, DataStructure& ds)
{
	string line;
	ifstream inputFile(path.c_str());

	if (inputFile.is_open())
	{
		int contRow = 0;
		float pointX, pointY;
		while (getline(inputFile, line))
		{
			int data, noData;
			regex_t dataExp, dataNExp;
			data = regcomp(&dataExp, "(\\w+)\\W+(.+(\\..+)?)", REG_EXTENDED);
			noData = regcomp(&dataNExp, "(\\w+)\\W+(-.+(\\..+)?)", REG_EXTENDED);
			if (data != 0 || noData != 0)
			{
				printf("regcomp failed with %d\n", data);
			}
			string temp = line;
			regmatch_t matches[MAX_MATCHES];
			if (match(&dataNExp, temp, matches))
			{
				string a = temp.substr(matches[1].rm_so, (matches[1].rm_eo - matches[1].rm_so));
				string b = temp.substr(matches[2].rm_so, (matches[2].rm_eo - matches[2].rm_so));
				if (a == "ncols")
					ds.setCols(strtof((b).c_str(), 0));
				else if (a == "nrows")
					ds.setRows(strtof((b).c_str(), 0));
				else if (a == "xllcorner")
					pointX = strtof((b).c_str(), 0);
				else if (a == "yllcorner")
					pointY = strtof((b).c_str(), 0);
				else if (a == "cellsize")
					ds.setCellSize(strtof((b).c_str(), 0));
				else if (a == "NODATA_value")
					ds.setNoData(strtof((b).c_str(), 0));
				else
					ds.addRow(temp);
				ds.setLeftCorner(pointX, pointY);
			}
			else if (match(&dataExp, temp, matches))
			{
				string a = temp.substr(matches[1].rm_so, (matches[1].rm_eo - matches[1].rm_so));
				string b = temp.substr(matches[2].rm_so, (matches[2].rm_eo - matches[2].rm_so));
				if (a == "ncols")
					ds.setCols(strtof((b).c_str(), 0));
				else if (a == "nrows")
					ds.setRows(strtof((b).c_str(), 0));
				else if (a == "xllcorner")
					pointX = strtof((b).c_str(), 0);
				else if (a == "yllcorner")
					pointY = strtof((b).c_str(), 0);
				else if (a == "cellsize")
					ds.setCellSize(strtof((b).c_str(), 0));
				else if (a == "NODATA_value")
					ds.setNoData(strtof((b).c_str(), 0));
				else
					ds.addRow(temp);
				ds.setLeftCorner(pointX, pointY);
			}
			regfree(&dataExp);
			regfree(&dataNExp);
		}
		inputFile.close();
	}

	else
		cout << "Unable to open file";
}

bool match(regex_t *pexp, string sz, regmatch_t matches[])
{

	if (regexec(pexp, sz.c_str(), MAX_MATCHES, matches, 0) == 0)
	{
		return true;
	}
	else
		return false;
}
unsigned int loadTexture(const char * path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}
