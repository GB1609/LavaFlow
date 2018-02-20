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
#include <string>
#define STB_IMAGE_IMPLEMENTATION
#include "../lib/stb_image.h"
#include "camera.h"
#include "dataStructure.h"
using namespace std;
using namespace glm;
#define MAX_MATCHES 3 //The maximum number of matches allowed in a single string
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void readFile(string path, DataStructure& ds);
bool match(regex_t *pexp, string sz, regmatch_t matches[]);
unsigned int loadTexture(const char * path);
int stepProject = 0;

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;

// camera
glm::vec3 cameraPosFirst = glm::vec3(69.4f, 71.8f, 20.9f);
glm::vec3 cameraPosSecond = glm::vec3(0.0f, 0.0f, 100.0f);

bool firstMouse = true;
bool moved = false;
bool Pressed = true;
Camera cam1(cameraPosFirst);
//Camera cam2(cameraPosSecond, glm::vec3(-0.08f, -0.58f, -0.82f), glm::vec3(-0.06f, -0.82f, -0.60f), -94.0f, -40.0f);
Camera cam2(cameraPosSecond);
float lastX = SCR_WIDTH / 2.0;
float lastY = SCR_HEIGHT / 2.0;
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
	//////////////////////////////DATE///////////////////////////////////////////////
	DataStructure topography;
	DataStructure lavaThickness;
	DataStructure lavaTemp;
//	readFile("Data/altitudes.dat", topography);
//	readFile("Data/lava.dat", lavaThickness);
//	readFile("Data/temperature.dat", lavaTemp);

	readFile("Data/lavaTest.dat", lavaThickness);
	readFile("Data/tempTest.dat", lavaTemp);
	readFile("Data/DEM_test.dat", topography);
//	readFile("Data/DEM_Albano.asc", dataSource);
	//////////////////////////////DATE///////////////////////////////////////////////


	////////////////////////////floor///////////////////////////////////////////////
//	vector<vector<float> > vertexFloor;
//	unsigned int VBOfloor[dataSource.getRows()];
//	unsigned int VAOfloor[dataSource.getRows()];
//	unsigned int VBOfloorTexture[dataSource.getRows()];
//	for (int i = 0; i < dataSource.getRows() - 1; i++)
//	{
//		vector<float> temp;
//		vector<float> tempTexture;
//		for (int j = 0; j < dataSource.getCols() - 1; j++)
//		{
//			dataSource.generatevertex(temp, i, j);
//		}
//		vertexFloor.push_back(temp);
//
//		glGenVertexArrays(1, &VAOfloor[i]);
//		glGenBuffers(1, &VBOfloor[i]);
//		glBindVertexArray(VAOfloor[i]);
//		glBindBuffer(GL_ARRAY_BUFFER, VBOfloor[i]);
//		glBufferData(GL_ARRAY_BUFFER, vertexFloor[i].size() * sizeof(float), &vertexFloor[i][0], GL_STATIC_DRAW);
//		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) 0);
//		glEnableVertexAttribArray(0);
//		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) (3 * sizeof(float)));
//		glEnableVertexAttribArray(1);
//
//	}

	////////////////////////////floor///////////////////////////////////////////////

	//////////////////////////2step///////////////////////////////////////

	/////aggiungo la thickenss alla matrice topografica
	topography.addThickens(lavaThickness);

	vector<float> fVertex, normali, textures, color;
	vector<unsigned int> finalIndex;
	topography.constructGrid(fVertex, finalIndex, normali, textures);

//	unsigned int VAOfloor, VBOfloor, EBOfloor, COLfloor;
//	glGenVertexArrays(1, &VAOfloor);
//	glGenBuffers(1, &VBOfloor);
//	glGenBuffers(1, &COLfloor);
//	glGenBuffers(1, &EBOfloor);
//	glBindVertexArray(VAOfloor);
//	glBindBuffer(GL_ARRAY_BUFFER, VBOfloor);
//	glBufferData(GL_ARRAY_BUFFER, fVertex.size() * sizeof(float), &fVertex[0], GL_STATIC_DRAW);
//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);
//	glEnableVertexAttribArray(0);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOfloor);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, finalIndex.size() * sizeof(int), &finalIndex[0], GL_STATIC_DRAW);
//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);
//	glEnableVertexAttribArray(0);
//	glBindBuffer(GL_ARRAY_BUFFER, COLfloor);
//	glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(float), &color[0], GL_STATIC_DRAW);
//	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) (0 * sizeof(float)));
//	glEnableVertexAttribArray(1);

	unsigned int VAO, VBO, EBO, NORMAL, TEXTURES;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glGenBuffers(1, &NORMAL);
	glGenBuffers(1, &TEXTURES);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, fVertex.size() * sizeof(float), &fVertex[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, finalIndex.size() * sizeof(int), &finalIndex[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, NORMAL);
	glBufferData(GL_ARRAY_BUFFER, normali.size() * sizeof(float), &normali[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) (0 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, TEXTURES);
	glBufferData(GL_ARRAY_BUFFER, textures.size() * sizeof(float), &textures[0], GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*) (0 * sizeof(float)));
	glEnableVertexAttribArray(2);

//	dataSource.printVertex(normali);
//	dataSource.printText(textures);
//	dataSource.printVertex(fVertex); //sono corretti
//	dataSource.printIndex(finalIndex, fVertex);
	////////////////texture/////////////
	unsigned int textID = loadTexture("Data/texture.png");
	////////////////texture/////////////

	//////////////////////////2step///////////////////////////////////////

	///////////////LOAD SHADER////////////////////////
	Shader lightShader("src/lightShader.vs", "src/lightShader.fs");
	Shader objShader("src/objects.vs", "src/objects.fs");
	////////////////////////////SHDAERS//////////////////////////////
//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//	glEnable(GL_DEPTH_TEST);
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
//		if (stepProject == 1)
//		{
//			if (Pressed)
//			{
//				cout << "STEP 1" << endl;
//				Pressed = false;
//			}
//			objShader.use();
//			glm::mat4 modelL;
//			//glm::mat4 projectionProspective = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 100.0f);
//			glm::mat4 projectionL = glm::perspective(glm::radians(cam1.Zoom), (float) SCR_WIDTH / (float) SCR_HEIGHT,
//					0.1f, 1000.0f);
//			glm::mat4 viewL = cam1.GetViewMatrix();
//			objShader.setMat4("projection", projectionL);
//			objShader.setMat4("view", viewL);
//			objShader.setMat4("model", modelL);
////			for (int i = 0; i < dataSource.getRows(); i++)
////			{
//
//			glBindVertexArray(VAOfloor);
//			objShader.setMat4("model", modelL);
//			glDrawElements( GL_TRIANGLES, finalIndex.size(), GL_UNSIGNED_INT, 0);
////			}
//		}
//		if (stepProject == 2)
		if (true)
		{
			if (Pressed)
			{
				cout << "STEP 2" << endl;
				Pressed = false;
			}
//			projectionL = glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f, 0.1f, 1000.0f);
			lightShader.use();
			glm::mat4 projection2 = glm::perspective(cam2.Zoom, (float) SCR_WIDTH / (float) SCR_HEIGHT,
					0.1f, 10000.0f);
			glm::mat4 view2 = cam2.GetViewMatrix();
			glm::mat4 model2 = glm::mat4();
			model2 = glm::translate(model2, glm::vec3(1.0f, 0.0f, 1.0f));
			lightShader.setMat4("projection", projection2);
			lightShader.setMat4("view", view2);
			lightShader.setMat4("model", model2);
			lightShader.setVec3("lightPos", cam2.Position);
			lightShader.setVec3("viewPos", cam2.Position);
			glBindVertexArray(VAO);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textID);
			glDrawElements( GL_TRIANGLES, finalIndex.size(), GL_UNSIGNED_INT, 0);
		}
		else if (Pressed)
		{
			cout << "waiting....... press 1 or 2" << endl;
			Pressed = false;
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &EBO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &NORMAL);
	glDeleteBuffers(1, &TEXTURES);
//	for (int i = 0; i < dataSource.getRows(); i++)
//	{
//		glDeleteVertexArrays(1, &VAOfloor[i]);
//		glDeleteBuffers(1, &VBOfloor[i]);
//	}

	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}
void processInput(GLFWwindow *window)
{

	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
	{
		stepProject = 1;
		Pressed = true;
	}

	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
	{
		stepProject = 2;
		Pressed = true;
	}

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
	{
		cout << "camInfo1" << endl;
		cout << "yaw:" << cam1.Yaw << "  " << "Pitch:" << cam1.Pitch << endl;
		cout << cam1.Position.x << "/" << cam1.Position.y << "/" << cam1.Position.z << endl;
		cout << cam1.Front.x << "/" << cam1.Front.y << "/" << cam1.Front.z << endl;
		cout << cam1.Up.x << "/" << cam1.Up.y << "/" << cam1.Up.z << endl;
		cout << "camInfo2" << endl;
		cout << "yaw:" << cam2.Yaw << "  " << "Pitch:" << cam2.Pitch << endl;
		cout << cam2.Position.x << "/" << cam2.Position.y << "/" << cam2.Position.z << endl;
		cout << cam2.Front.x << "/" << cam2.Front.y << "/" << cam2.Front.z << endl;
		cout << cam2.Up.x << "/" << cam2.Up.y << "/" << cam2.Up.z << endl;
	}

	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
	{
		if (stepProject == 1)
			cam1.resetVisual();
		else
			cam2.resetVisual();
		moved = true;
	}

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		if (stepProject == 1)
			cam1.ProcessKeyboard(FORWARD, deltaTime);
		else
			cam2.ProcessKeyboard(FORWARD, deltaTime);
		moved = true;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		if (stepProject == 1)
			cam1.ProcessKeyboard(BACKWARD, deltaTime);
		else
			cam2.ProcessKeyboard(BACKWARD, deltaTime);
		moved = true;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		if (stepProject == 1)
			cam1.ProcessKeyboard(LEFT, deltaTime);
		else
			cam2.ProcessKeyboard(LEFT, deltaTime);
		moved = true;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		if (stepProject == 1)
			cam1.ProcessKeyboard(RIGHT, deltaTime);
		else
			cam2.ProcessKeyboard(RIGHT, deltaTime);
		moved = true;
	}

}
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS)
	{

		if (firstMouse || moved)
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
		if (stepProject == 1)
			cam1.ProcessMouseMovement(xoffset, yoffset);
		else
			cam2.ProcessMouseMovement(xoffset, yoffset);
	}
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (stepProject == 1)
		cam1.ProcessMouseScroll(yoffset);
	else
		cam2.ProcessMouseScroll(yoffset);
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
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
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
