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
//glm::vec3 cameraPos = glm::vec3(21.9f, 13.013f, 22.2f); //per test
glm::vec3 cameraPos = glm::vec3(708.0f, 648.0f, 6675.0f);
glm::vec3 lightPos = glm::vec3(0.0f, 9.0f, 7000.0f);
Camera cam(cameraPos);
bool firstMouse = true;
bool moved = false;
bool Pressed = true;

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
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LAVAFLOW", NULL, NULL);
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
	readFile("Data/altitudes.dat", topography);
	readFile("Data/lava.dat", lavaThickness);
	readFile("Data/temperature.dat", lavaTemp);

//	readFile("Data/lavaTest.dat", lavaThickness);
//	readFile("Data/tempTest.dat", lavaTemp);
//	readFile("Data/DEM_test.dat", topography);
////	readFile("Data/DEM_Albano.asc", dataSource);
	//////////////////////////////DATE///////////////////////////////////////////////

	if ((lavaThickness.getCols() != lavaTemp.getCols()) || (lavaThickness.getCols() != topography.getCols())
			|| (lavaThickness.getRows() != lavaTemp.getRows()) || (lavaThickness.getRows() != topography.getRows()))
	{
		cout << "le dimensioni dei file non corrispondono, verifica di aver scelto i file corretti" << endl;
		glfwTerminate();
	}

	///////////////LOAD SHADER////////////////////////
	Shader lightShader("src/lightShader.vs", "src/lightShader.fs");
	Shader objShader("src/objects.vs", "src/objects.fs");
	////////////////////////////SHDAERS//////////////////////////////

	/////aggiungo la thickenss alla matrice topografica

	topography.addThickness(lavaThickness);
	topography.setMax();
	lavaTemp.setMax();
	topography.constructSUPPORT(lavaTemp);

	////////////////////////////floor///////////////////////////////////////////////
	vector<vector<float> > vertexFloor;
	vector<vector<float> > vertexColor;
	vector<vector<unsigned int> > indexFloor;
	unsigned int VBOfloor[topography.getRows() - 1];
	unsigned int VAOfloor[topography.getRows() - 1];
	unsigned int EBOfloor[topography.getRows() - 1];
	unsigned int COLORfloor[topography.getRows() - 1];
	unsigned int VBOfloorTexture[topography.getRows()];
	for (int i = 0; i < topography.getRows() - 1; i++)
	{
		vector<float> tempVertex;
		vector<unsigned int> tempIndex;
		vector<float> color;
		for (int j = 0; j < topography.getCols() - 1; j++)
			topography.generateRowFirstStep(tempVertex, color, i, j);
		topography.generateIndexFirstStep(tempIndex);
		vertexFloor.push_back(tempVertex);
		vertexColor.push_back(color);
		indexFloor.push_back(tempIndex);
		glGenVertexArrays(1, &VAOfloor[i]);
		glGenBuffers(1, &VBOfloor[i]);
		glGenBuffers(1, &EBOfloor[i]);
		glGenBuffers(1, &COLORfloor[i]);
		glBindVertexArray(VAOfloor[i]);
		glBindBuffer(GL_ARRAY_BUFFER, VBOfloor[i]);
		glBufferData(GL_ARRAY_BUFFER, vertexFloor[i].size() * sizeof(float), &vertexFloor[i][0], GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOfloor[i]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexFloor[i].size() * sizeof(int), &indexFloor[i][0], GL_STATIC_DRAW);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);
		glEnableVertexAttribArray(4);
		glBindBuffer(GL_ARRAY_BUFFER, COLORfloor[i]);
		glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(float), &color[0], GL_STATIC_DRAW);
		glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) (0 * sizeof(float)));
		glEnableVertexAttribArray(5);

	}
	////////////////////////////floor///////////////////////////////////////////////

	//////////////////////////2step///////////////////////////////////////

	vector<float> fVertex, normali, textures, colorTemp;
	vector<unsigned int> finalIndex;
	topography.constructAll(fVertex, finalIndex, normali, textures, lavaTemp, colorTemp);

	unsigned int VAO, VBO, EBO, NORMAL, TEXTURES, TEMPERATURE;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glGenBuffers(1, &NORMAL);
	glGenBuffers(1, &TEXTURES);
	glGenBuffers(1, &TEMPERATURE);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, fVertex.size() * sizeof(float), &fVertex[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, finalIndex.size() * sizeof(int), &finalIndex[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) (0 * sizeof(float)));
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, NORMAL);
	glBufferData(GL_ARRAY_BUFFER, normali.size() * sizeof(float), &normali[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) (0 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, TEXTURES);
	glBufferData(GL_ARRAY_BUFFER, textures.size() * sizeof(float), &textures[0], GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*) (0 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, TEMPERATURE);
	glBufferData(GL_ARRAY_BUFFER, colorTemp.size() * sizeof(float), &colorTemp[0], GL_STATIC_DRAW);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 1 * sizeof(float), (void*) (0 * sizeof(float)));
	glEnableVertexAttribArray(3);
	////////////////texture/////////////
	unsigned int textID = loadTexture("Data/texture.png");


	//////////////////////////2step///////////////////////////////////////

//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//	glEnable(GL_DEPTH_TEST);
//	glDepthFunc(GL_ALWAYS);
//	glEnable(GL_BLEND);
//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	while (!glfwWindowShouldClose(window))
	{
		currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		processInput(window);
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		if (stepProject == 1)
		{
			if (Pressed)
			{
				cout << "STEP 1" << endl;
				Pressed = false;
			}
			objShader.use();
			glm::mat4 modelL = glm::mat4();
			modelL = glm::translate(modelL, glm::vec3(1.0f, 0.0f, 1.0f));
			//glm::mat4 projectionProspective = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 100.0f);
			glm::mat4 projectionL = glm::perspective(glm::radians(cam.Zoom), (float) SCR_WIDTH / (float) SCR_HEIGHT,
					0.1f, 10000.0f);
			glm::mat4 viewL = cam.GetViewMatrix();

			for (int i = 0; i < topography.getRows(); i++)
			{
				objShader.setMat4("projection", projectionL);
				objShader.setMat4("view", viewL);
				objShader.setMat4("model", modelL);
				glBindVertexArray(VAOfloor[i]);

				glDrawElements( GL_TRIANGLES, indexFloor[i].size(), GL_UNSIGNED_INT, 0);
			}
		}
		if (stepProject == 2 || stepProject == 3)
		{
			if (Pressed)
			{
				cout << "STEP 2" << endl;
				Pressed = false;
			}
//			projectionL = glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f, 0.1f, 1000.0f);
			lightShader.use();
			glm::mat4 projection2 = glm::perspective(cam.Zoom, (float) SCR_WIDTH / (float) SCR_HEIGHT, 0.1f, 10000.0f);
			glm::mat4 view2 = cam.GetViewMatrix();
			glm::mat4 model2 = glm::mat4();
			model2 = glm::translate(model2, glm::vec3(1.0f, 0.0f, 1.0f));
			lightShader.setMat4("projection", projection2);
			lightShader.setMat4("view", view2);
			lightShader.setMat4("model", model2);
			lightShader.setVec3("lightPos", lightPos);
			lightShader.setVec3("viewPos", cam.Position);
			glBindVertexArray(VAO);
			if (stepProject == 3)
			{
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, textID);
			}
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
	for (int i = 0; i < topography.getRows(); i++)
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

	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
	{
		stepProject = 1;
		Pressed = true;
	}

	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
	{
		stepProject = 2;
		glBindTexture(GL_TEXTURE_2D, 0);
		Pressed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
	{
		stepProject = 3;
		Pressed = true;
	}

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
	{
		cout << "camInfo2" << endl;
		cout << "yaw:" << cam.Yaw << "  " << "Pitch:" << cam.Pitch << endl;
		cout << cam.Position.x << "/" << cam.Position.y << "/" << cam.Position.z << endl;
		cout << cam.Front.x << "/" << cam.Front.y << "/" << cam.Front.z << endl;
		cout << cam.Up.x << "/" << cam.Up.y << "/" << cam.Up.z << endl;
	}

	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
	{
		cam.resetVisual();
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
		cam.ProcessMouseMovement(xoffset, yoffset);
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
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		stbi_set_flip_vertically_on_load(true);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}
