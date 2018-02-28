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
void processInput(GLFWwindow *window);
void readFile(string path, DataStructure& ds);
bool match(regex_t *pexp, string sz, regmatch_t matches[]);
unsigned int loadTexture(const char * path);
int stepProject = 0;

float angleRotationX = 0, angleRotationY = 0;

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;

//glm::vec3 cameraPos = glm::vec3(21.9f, 17.013f, 215.2f); //per testdataset
//glm::vec3 lightPos = glm::vec3(0.0f, 9.0f, 60.0f); //for test dataset
///comment next line and decomment previously for dataset test
glm::vec3 cameraPos = glm::vec3(1108.0f, 648.0f, 6675.0f);
glm::vec3 lightPos = glm::vec3(-50.0f, 1300.0f, 2800.0f);

Camera light(lightPos);
Camera cam(cameraPos);
bool firstMouse = true;
bool moved = false;

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

	///comment to use test dataset
	readFile("Data/altitudes.dat", topography);
	readFile("Data/lava.dat", lavaThickness);
	readFile("Data/temperature.dat", lavaTemp);

// decomment for use test dataset
//	readFile("Data/lavaTest.dat", lavaThickness);
//	readFile("Data/tempTest.dat", lavaTemp);
//	readFile("Data/DEM_test.dat", topography);
	//////////////////////////////DATE///////////////////////////////////////////////

	/////////add the thickness
	if ((lavaThickness.getCols() != lavaTemp.getCols()) || (lavaThickness.getCols() != topography.getCols())
			|| (lavaThickness.getRows() != lavaTemp.getRows()) || (lavaThickness.getRows() != topography.getRows()))
	{
		cout << "dimension of file not correspond, verify that you have chosen the correct files" << endl;
		glfwTerminate();
	}
	topography.addThickness(lavaThickness);
	/////////support function
	topography.setMax();
	topography.setMin();
	lavaTemp.setMax();
	lavaTemp.setMin();
	topography.constructSUPPORT(lavaTemp);

	///////////////LOAD SHADER////////////////////////
	Shader lightShader("src/lightShader.vs", "src/lightShader.fs"); // for view with texture and light effect
	Shader straightShader("src/straight.vs", "src/straight.fs"); //for straightforward color mapping
	////////////////////////////SHDAERS//////////////////////////////

	////////////////////////////STEP1///////////////////////////////////////////////
	vector<vector<float> > vertexStraightFloor;
	vector<vector<float> > vertexStraightColor;
	vector<vector<unsigned int> > indexStraight;
	unsigned int VBOstraight[topography.getRows() - 1];
	unsigned int VAOstraight[topography.getRows() - 1];
	unsigned int EBOstraight[topography.getRows() - 1];
	unsigned int COLORstraight[topography.getRows() - 1];
	bool asa = false;
	for (int i = 0; i < topography.getRows() - 1; i++)
	{
		vector<float> straightVertex;
		vector<unsigned int> straightIndex;
		vector<float> straightColor;
		for (int j = 0; j < topography.getCols() - 1; j++)
			topography.generateRowFirstStep(straightVertex, straightColor, i, j);
		topography.generateIndexFirstStep(straightIndex);
		vertexStraightFloor.push_back(straightVertex);
		vertexStraightColor.push_back(straightColor);
		indexStraight.push_back(straightIndex);
		///fino a qua arivano giusto, la funzione giu commentata stampa i colori con i valori corretti.
//		topography.printVertex(vertexStraightColor[i]);

		glGenVertexArrays(1, &VAOstraight[i]);
		glGenBuffers(1, &VBOstraight[i]);
		glGenBuffers(1, &EBOstraight[i]);
		glGenBuffers(1, &COLORstraight[i]);
		glBindVertexArray(VAOstraight[i]);
		glBindBuffer(GL_ARRAY_BUFFER, VBOstraight[i]);
		glBufferData(GL_ARRAY_BUFFER, vertexStraightFloor[i].size() * sizeof(float), &vertexStraightFloor[i][0],
		GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOstraight[i]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexStraight[i].size() * sizeof(int), &indexStraight[i][0],
		GL_STATIC_DRAW);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) (0 * sizeof(float)));
		glEnableVertexAttribArray(4);
		glBindBuffer(GL_ARRAY_BUFFER, COLORstraight[i]);
		glBufferData(GL_ARRAY_BUFFER, vertexStraightColor[i].size() * sizeof(float), &vertexStraightColor[i][0],
		GL_STATIC_DRAW);
		glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) (0 * sizeof(float)));
		glEnableVertexAttribArray(5);

	}
	////////////////////////////SPTEP1///////////////////////////////////////////////

	//////////////////////////2step///////////////////////////////////////

	vector<float> mapViewVertex, mapViewNormal, mapViewTextures, mapViewColorTemp;
	vector<unsigned int> finalIndex;
	topography.constructAll(mapViewVertex, finalIndex, mapViewNormal, mapViewTextures, lavaTemp, mapViewColorTemp);

	unsigned int VAO, VBO, EBO, NORMAL, TEXTURES, TEMPERATURE;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glGenBuffers(1, &NORMAL);
	glGenBuffers(1, &TEXTURES);
	glGenBuffers(1, &TEMPERATURE);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, mapViewVertex.size() * sizeof(float), &mapViewVertex[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, finalIndex.size() * sizeof(int), &finalIndex[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) (0 * sizeof(float)));
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, NORMAL);
	glBufferData(GL_ARRAY_BUFFER, mapViewNormal.size() * sizeof(float), &mapViewNormal[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) (0 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, TEXTURES);
	glBufferData(GL_ARRAY_BUFFER, mapViewTextures.size() * sizeof(float), &mapViewTextures[0], GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*) (0 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, TEMPERATURE);
	glBufferData(GL_ARRAY_BUFFER, mapViewColorTemp.size() * sizeof(float), &mapViewColorTemp[0], GL_STATIC_DRAW);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 1 * sizeof(float), (void*) (0 * sizeof(float)));
	glEnableVertexAttribArray(3);
	////////////////texture/////////////
	unsigned int textID = loadTexture("Data/texture.png");
	lightShader.setInt("TEXTURE", 0);
	//////////////////////////2step///////////////////////////////////////

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
			straightShader.use();
			glm::mat4 model1 = glm::mat4();
			model1 = glm::translate(model1, glm::vec3(1.0f, 0.0f, 1.0f));
			model1 = glm::rotate(model1, glm::radians(angleRotationX), glm::vec3(1.0f, 1.0f, 0.0f));
			model1 = glm::rotate(model1, glm::radians(angleRotationY), glm::vec3(0.0f, 1.0f, 1.0f));
			glm::mat4 projection1 = glm::perspective(glm::radians(cam.Zoom), (float) SCR_WIDTH / (float) SCR_HEIGHT,
					0.1f, 10000.0f);
			glm::mat4 view1 = cam.GetViewMatrix();
			straightShader.setMat4("projection", projection1);
			straightShader.setMat4("view", view1);
			straightShader.setMat4("model", model1);
			for (int i = 0; i < topography.getRows(); i++)
			{
				glBindVertexArray(VAOstraight[i]);
				glDrawElements( GL_TRIANGLES, indexStraight[i].size(), GL_UNSIGNED_INT, 0);
			}
		}
		if (stepProject == 2 || stepProject == 3 || stepProject == 4)
		{
			lightShader.use();
			glm::mat4 projection2 = glm::perspective(cam.Zoom, (float) SCR_WIDTH / (float) SCR_HEIGHT, 0.1f, 10000.0f);
			glm::mat4 view2 = cam.GetViewMatrix();
			glm::mat4 model2 = glm::mat4();
			model2 = glm::translate(model2, glm::vec3(1.0f, 0.0f, 1.0f));
			model2 = glm::rotate(model2, glm::radians(angleRotationX), glm::vec3(1.0f, 1.0f, 0.0f));
			model2 = glm::rotate(model2, glm::radians(angleRotationY), glm::vec3(0.0f, 1.0f, 1.0f));
			lightShader.setBool("textured", false);
			lightShader.setBool("lava", false);
			lightShader.setMat4("projection", projection2);
			lightShader.setMat4("view", view2);
			lightShader.setMat4("model", model2);
			lightShader.setVec3("lightPos", light.Position);
			lightShader.setVec3("viewPos", cam.Position);
			glBindVertexArray(VAO);
			if (stepProject == 3)
			{
				lightShader.setBool("lava", true);
			}
			if (stepProject == 4)
			{
				glBindTexture(GL_TEXTURE_2D, textID);
				lightShader.setBool("lava", true);
				lightShader.setBool("textured", true);
			}

			glDrawElements( GL_TRIANGLES, finalIndex.size(), GL_UNSIGNED_INT, 0);
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
		glDeleteVertexArrays(1, &VAOstraight[i]);
		glDeleteBuffers(1, &VBOstraight[i]);
		glDeleteBuffers(1, &EBOstraight[i]);
		glDeleteBuffers(1, &COLORstraight[i]);
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
		stepProject = 1;

	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
		stepProject = 2;
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
		stepProject = 3;
	if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
		stepProject = 4;

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
	{
		cout << "camInfo" << endl;
		cout << "yaw:" << cam.Yaw << "  " << "Pitch:" << cam.Pitch << endl;
		cout << cam.Position.x << "/" << cam.Position.y << "/" << cam.Position.z << endl;
		cout << cam.Front.x << "/" << cam.Front.y << "/" << cam.Front.z << endl;
		cout << cam.Up.x << "/" << cam.Up.y << "/" << cam.Up.z << endl;
		cout << "lightInfo" << endl;
		cout << "yaw:" << light.Yaw << "  " << "Pitch:" << light.Pitch << endl;
		cout << light.Position.x << "/" << light.Position.y << "/" << light.Position.z << endl;
		cout << light.Front.x << "/" << light.Front.y << "/" << light.Front.z << endl;
		cout << light.Up.x << "/" << light.Up.y << "/" << light.Up.z << endl;
	}

	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
	{
		cam.resetVisual();
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		angleRotationX = angleRotationY = 0.0f;
	}
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_ALT) != GLFW_PRESS)
	{
		if (angleRotationX + 0.2 < 30)
			angleRotationX += 0.2f;
	}
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_ALT) != GLFW_PRESS)
	{
		if (angleRotationX - 0.2 > -30)
			angleRotationX -= 0.2f;
	}
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS)
	{
		if (angleRotationY + 0.2 < 30)
			angleRotationY += 0.2f;
	}
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS)
	{
		if (angleRotationY - 0.2 > -30)
			angleRotationY -= 0.2f;
	}
	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
	{
		light.resetVisual();
	}

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_ALT) != GLFW_PRESS)
	{
		cam.ProcessKeyboard(FORWARD, deltaTime);
		moved = true;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_ALT) != GLFW_PRESS)
	{
		cam.ProcessKeyboard(BACKWARD, deltaTime);
		moved = true;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_ALT) != GLFW_PRESS)
	{
		cam.ProcessKeyboard(LEFT, deltaTime);
		moved = true;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_ALT) != GLFW_PRESS)
	{
		cam.ProcessKeyboard(RIGHT, deltaTime);
		moved = true;
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS)
	{
		light.ProcessKeyboard(FORWARD, deltaTime);
		moved = true;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS)
	{
		light.ProcessKeyboard(BACKWARD, deltaTime);
		moved = true;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS)
	{
		light.ProcessKeyboard(LEFT, deltaTime);
		moved = true;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS)
	{
		light.ProcessKeyboard(RIGHT, deltaTime);
		moved = true;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_ALT) != GLFW_PRESS)
	{
		cam.ProcessKeyboard(ROTATELEFT, deltaTime);
		moved = true;
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_ALT) != GLFW_PRESS)
	{
		cam.ProcessKeyboard(ROTATERIGHT, deltaTime);
		moved = true;
	}
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_ALT) != GLFW_PRESS)
	{
		cam.ProcessKeyboard(ROTATEUP, deltaTime);
		moved = true;
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_ALT) != GLFW_PRESS)
	{
		cam.ProcessKeyboard(ROTATEDOWN, deltaTime);
		moved = true;
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS)
	{
		light.ProcessKeyboard(ROTATELEFT, deltaTime);
		moved = true;
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS)
	{
		light.ProcessKeyboard(ROTATERIGHT, deltaTime);
		moved = true;
	}
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS)
	{
		light.ProcessKeyboard(ROTATEUP, deltaTime);
		moved = true;
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS)
	{
		light.ProcessKeyboard(ROTATEDOWN, deltaTime);
		moved = true;
	}

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
