#include <iostream>
using namespace std;


//--- OpenGL ---
#include "GL\glew.h"
#include "GL\wglew.h"
#pragma comment(lib, "glew32.lib")
//--------------
#include "GLFW\glfw3.h";

#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtc\type_ptr.hpp"
#include "glm\gtc\matrix_inverse.hpp"

#include "GL\freeglut.h"

#include "Images\FreeImage.h"

#include "shaders\Shader.h"

#include "Octree\Octree.h"

CShader* myShader;  ///shader object 
CShader* myBasicShader;

//MODEL LOADING
#include "3DStruct\threeDModel.h"
#include "Obj\OBJLoader.h"

float amount = 0;
float temp = 0.002f;
int cameraType = 0;

CThreeDModel Horse,Horse2,Horse3,Horse4,grassFloor,carousel1,carousel2,carousel3,Terrain, tower1, tower2, tower3, tower4, castleWall1, castleWall2,castleWall3,castleWall4; //A threeDModel object is needed for each model loaded
COBJLoader objLoader;	//this object is used to load the 3d models.
///END MODEL LOADING

glm::mat4 ProjectionMatrix; // matrix for the orthographic projection
glm::mat4 ModelViewMatrix;  // matrix for the modelling and viewing

glm::mat4 objectRotation;
glm::vec3 translation = glm::vec3(0.0, 0.0, 0.0);
glm::vec3 pos = glm::vec3(0.0f,0.0f,0.0f); //vector for the position of the object.
glm::vec3 pos2 = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 pos3 = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 pos4 = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 camPos = glm::vec3(6.0f, 4.0f, 0.0f);
glm::vec3 camFront = glm::vec3(0.0f, 0.0f, 1.0f);
glm::vec3 camUp = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 lastCamPos = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 lastCamPos2 = glm::vec3(10.0f, 0.0f, 0.0f);

float horseAngle = 0.0f;
float bounceAngle = 0.0f;
float rotateAngle = PI;
float camRight = 0.0;
float camFoward = 0.0;
float lightAngle = 0.f;

float R = 1.0f;
float G = 1.0f;
float B = 1.0f;

float xoffset = 0.0f;
float yoffset = 0.0f;
float zoffset = 0.0f;

//Material properties

float Material_Specular[4] = {0.8f,0.8f,0.8f,1.0f};
float Material_Shininess = 50;

//Light Properties

float Light_Specular[4] = { 0.8f,0.8f,0.8f,0.8f};
float LightPos[4] = {0.0f, 1.0f, 0.0f, 0.0f};

//
int	mouse_x=0, mouse_y=0;
bool LeftPressed = false;
int screenWidth=800, screenHeight=600;
float lastX = 400, lastY = 300;
float yaw = -90.0f;
float pitch = 0.0f;
int count2 = 0;
float rideSpeed = 1;
float discoCount = 0;

//booleans to handle when the arrow keys are pressed or released.
bool Left = false;
bool Right = false;
bool Up = false;
bool Down = false;
bool Home = false;
bool End = false;
bool upOn = false;
bool rightOn = false;
bool firstLoop = true;
bool firstmouse = true;
bool collision = true;
bool pause = false;
bool disco = false;
double maxX,maxY,maxZ,minX,minY,minZ;



float spin=180;
float speed=0;

//OPENGL FUNCTION PROTOTYPES
void display();				//called in winmain to draw everything to the screen
void reshape(int width, int height);				//called when the window is resized
void init();				//called in winmain when the program starts.
void processKeys();         //called in winmain to process keyboard input
void idle();		//idle function
void updateTransform(float xinc, float yinc, float zinc);
void DrawAndCheckCollision(CThreeDModel model);
/*************    START OF OPENGL FUNCTIONS   ****************/
void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(myShader->GetProgramObjID());  // use the shader

	//Part for displacement shader.
	amount += temp;
	if (amount > 1.0f || amount < -1.5f)
		temp = -temp;
	//amount = 0;
	glUniform1f(glGetUniformLocation(myShader->GetProgramObjID(), "displacement"), amount);

	//Set the projection matrix in the shader
	GLuint projMatLocation = glGetUniformLocation(myShader->GetProgramObjID(), "ProjectionMatrix");
	glUniformMatrix4fv(projMatLocation, 1, GL_FALSE, &ProjectionMatrix[0][0]);


	//----------------------------------------------------------------------
	if (!pause) {
		rotateAngle += 0.005f * rideSpeed;
		if (rotateAngle > 360.0)
			rotateAngle = 0;

		discoCount += 0.005f * rideSpeed;
		if (discoCount > 6)
			discoCount = 0;

		bounceAngle += 0.01f * rideSpeed;
		if (bounceAngle > 360) {
			bounceAngle = 0;
		}

		horseAngle += 0.005f * rideSpeed;
		if (horseAngle > 360.0)
			horseAngle = 0;
	}
	pos.y = 0.5 * sin(bounceAngle) + 0.5 ;
	pos2.y = 0.5 * sin(bounceAngle - 60) + 0.5;
	pos3.y = 0.5 * sin(bounceAngle - 300) + 0.5;
	pos4.y = 0.5 * sin(bounceAngle - 180) + 0.5;
	

	glm::mat4 viewingMatrix = glm::mat4(1.0f);
	
	
	
	if (cameraType == 0) {
		viewingMatrix = glm::lookAt(camPos, camPos + camFront, camUp);
	}
	else if (cameraType == 1) {
		collision = false;
		camPos.x = 3.5 * sin(rotateAngle - 0.2);
		camPos.z = 3.5 * cos(rotateAngle - 0.2);
		camPos.y = -pos.y -3.3;
		float cameraRotateAngle = -(rotateAngle + 0.5 * PI );
		viewingMatrix = glm::rotate(viewingMatrix, cameraRotateAngle, glm::vec3(0, 1, 0));
		viewingMatrix = glm::translate(viewingMatrix, camPos);
	}
	else if (cameraType == 2) {
		collision = false;
		camPos.x = 3.5 * sin(rotateAngle - 40.9);
		camPos.z = 3.5 * cos(rotateAngle - 40.9);
		camPos.y = pos.y + 3.72;
		viewingMatrix = glm::lookAt(camPos, camPos + camFront, camUp);
	}
	else if (cameraType == 3) {
		collision = true;
		camPos.y = 1.5;
		
		viewingMatrix = glm::lookAt(camPos, camPos + camFront, camUp);
	}



	LightPos[0] = 2.5 * sin(rotateAngle);
	LightPos[2] = 2.5 * cos(rotateAngle)+4;
	LightPos[1] = -4;
	
	

	if (disco) {

		
		if (discoCount < 1 && discoCount > 0)  {
			R = 0.0f;
			G = 1.0f;
			B = 0.0f;
			
		}
		else if (discoCount < 2 && discoCount > 1) {
			R = 1.0f;
			G = 1.0f;
			B = 0.0f;
			
		}
		else if (discoCount < 3 && discoCount > 2) {
			R = 1.0f;
			G = 0.0f;
			B = 1.0f;
			
		}
		else if (discoCount < 4 && discoCount > 3) {
			R = 0.0f;
			G = 0.0f;
			B = 1.0f;
		}
		else if (discoCount < 5 && discoCount > 4) {
			R = 0.0f;
			G = 1.0f;
			B = 1.0f;
		}
		else if (discoCount < 6 && discoCount > 5) {
			R = 1.0f;
			G = 0.0f;
			B = 0.0f;
		}
	}
	if (!disco) {
		R = 1.0f;
		G = 1.0f;
		B = 1.0f;
	}
	
	float Light_Ambient[4] = { R, G, B, 1.0f };
	float Light_Diffuse[4] = { R, G, B, 1.0f };

	float Material_Ambient[4] = { R, G, B, 1.0f };
	float Material_Diffuse[4] = { R, G, B, 1.0f };

	glUniform4fv(glGetUniformLocation(myShader->GetProgramObjID(), "LightPos"), 1, LightPos);
	glUniform4fv(glGetUniformLocation(myShader->GetProgramObjID(), "light_ambient"), 1, Light_Ambient);
	glUniform4fv(glGetUniformLocation(myShader->GetProgramObjID(), "light_diffuse"), 1, Light_Diffuse);
	glUniform4fv(glGetUniformLocation(myShader->GetProgramObjID(), "light_specular"), 1, Light_Specular);

	glUniform4fv(glGetUniformLocation(myShader->GetProgramObjID(), "material_ambient"), 1, Material_Ambient);
	glUniform4fv(glGetUniformLocation(myShader->GetProgramObjID(), "material_diffuse"), 1, Material_Diffuse);
	glUniform4fv(glGetUniformLocation(myShader->GetProgramObjID(), "material_specular"), 1, Material_Specular);
	glUniform1f(glGetUniformLocation(myShader->GetProgramObjID(), "material_shininess"), Material_Shininess);

	
	
	glUniformMatrix4fv(glGetUniformLocation(myShader->GetProgramObjID(), "ViewMatrix"), 1, GL_FALSE, &viewingMatrix[0][0]);
	
	
	

	
	
	
	glm::mat4 modelmatrix = glm::translate(glm::mat4(1.0f), pos);
	modelmatrix = glm::rotate(modelmatrix, rotateAngle, glm::vec3(0, 1, 0));
	ModelViewMatrix = viewingMatrix * modelmatrix;
	glUniformMatrix4fv(glGetUniformLocation(myShader->GetProgramObjID(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);
	glm::mat3 normalMatrix = glm::inverseTranspose(glm::mat3(ModelViewMatrix));
	glUniformMatrix3fv(glGetUniformLocation(myShader->GetProgramObjID(), "NormalMatrix"), 1, GL_FALSE, &normalMatrix[0][0]);

	
	Horse.DrawElementsUsingVBO(myShader);
	
	modelmatrix = glm::translate(glm::mat4(1.0f), pos2);
	modelmatrix = glm::rotate(modelmatrix, rotateAngle, glm::vec3(0, 1, 0));
	ModelViewMatrix = viewingMatrix * modelmatrix;
	glUniformMatrix4fv(glGetUniformLocation(myShader->GetProgramObjID(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);
	normalMatrix = glm::inverseTranspose(glm::mat3(ModelViewMatrix));
	glUniformMatrix3fv(glGetUniformLocation(myShader->GetProgramObjID(), "NormalMatrix"), 1, GL_FALSE, &normalMatrix[0][0]);
	Horse2.DrawElementsUsingVBO(myShader);

	

	modelmatrix = glm::translate(glm::mat4(1.0f), pos3);
	modelmatrix = glm::rotate(modelmatrix, rotateAngle, glm::vec3(0, 1, 0));
	ModelViewMatrix = viewingMatrix * modelmatrix;
	glUniformMatrix4fv(glGetUniformLocation(myShader->GetProgramObjID(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);
	normalMatrix = glm::inverseTranspose(glm::mat3(ModelViewMatrix));
	glUniformMatrix3fv(glGetUniformLocation(myShader->GetProgramObjID(), "NormalMatrix"), 1, GL_FALSE, &normalMatrix[0][0]);
	Horse3.DrawElementsUsingVBO(myShader);

	modelmatrix = glm::translate(glm::mat4(1.0f), pos4);
	modelmatrix = glm::rotate(modelmatrix, rotateAngle, glm::vec3(0, 1, 0));
	ModelViewMatrix = viewingMatrix * modelmatrix;
	glUniformMatrix4fv(glGetUniformLocation(myShader->GetProgramObjID(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);
	normalMatrix = glm::inverseTranspose(glm::mat3(ModelViewMatrix));
	glUniformMatrix3fv(glGetUniformLocation(myShader->GetProgramObjID(), "NormalMatrix"), 1, GL_FALSE, &normalMatrix[0][0]);

	Horse4.DrawElementsUsingVBO(myShader);

	yoffset = 0.5;

	modelmatrix = glm::translate(glm::mat4(1.0f), glm::vec3(xoffset,yoffset,zoffset));
	modelmatrix = glm::rotate(modelmatrix, rotateAngle, glm::vec3(0, 1, 0));
	ModelViewMatrix = viewingMatrix * modelmatrix;
	glUniformMatrix4fv(glGetUniformLocation(myShader->GetProgramObjID(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);
	normalMatrix = glm::inverseTranspose(glm::mat3(ModelViewMatrix));
	glUniformMatrix3fv(glGetUniformLocation(myShader->GetProgramObjID(), "NormalMatrix"), 1, GL_FALSE, &normalMatrix[0][0]);
	     
	
	DrawAndCheckCollision(carousel3);
	DrawAndCheckCollision(carousel2);
	yoffset = 1.5;
	DrawAndCheckCollision(carousel1);

	yoffset = 0.0f;
	modelmatrix = glm::translate(glm::mat4(1.0f), glm::vec3(xoffset, yoffset, zoffset));
	
	ModelViewMatrix = viewingMatrix * modelmatrix;
	glUniformMatrix4fv(glGetUniformLocation(myShader->GetProgramObjID(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);
	normalMatrix = glm::inverseTranspose(glm::mat3(ModelViewMatrix));
	glUniformMatrix3fv(glGetUniformLocation(myShader->GetProgramObjID(), "NormalMatrix"), 1, GL_FALSE, &normalMatrix[0][0]);

	DrawAndCheckCollision(tower1);
	DrawAndCheckCollision(tower2);
	DrawAndCheckCollision(tower3);
	DrawAndCheckCollision(tower4);
	DrawAndCheckCollision(castleWall1);
	DrawAndCheckCollision(castleWall2);
	DrawAndCheckCollision(castleWall3);
	DrawAndCheckCollision(castleWall4);
	DrawAndCheckCollision(grassFloor);

	

	
	
	
	//-------------------------------------------------------------------------------------------------------

	//Switch to basic shader to draw the lines for the bounding boxes
	glUseProgram(myBasicShader->GetProgramObjID());
	projMatLocation = glGetUniformLocation(myBasicShader->GetProgramObjID(), "ProjectionMatrix");
	glUniformMatrix4fv(projMatLocation, 1, GL_FALSE, &ProjectionMatrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(myBasicShader->GetProgramObjID(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);

	

	//switch back to the shader for textures and lighting on the objects.
	glUseProgram(myShader->GetProgramObjID());  // use the shader

	ModelViewMatrix = glm::translate(viewingMatrix, glm::vec3(0, 0, 0));
	glUniformMatrix4fv(glGetUniformLocation(myShader->GetProgramObjID(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);
	normalMatrix = glm::inverseTranspose(glm::mat3(ModelViewMatrix));
	glUniformMatrix3fv(glGetUniformLocation(myShader->GetProgramObjID(), "NormalMatrix"), 1, GL_FALSE, &normalMatrix[0][0]);
	
	ModelViewMatrix = glm::translate(viewingMatrix, glm::vec3(0, -5.2, 0));
	glUniformMatrix4fv(glGetUniformLocation(myShader->GetProgramObjID(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);
	normalMatrix = glm::inverseTranspose(glm::mat3(ModelViewMatrix));
	glUniformMatrix3fv(glGetUniformLocation(myShader->GetProgramObjID(), "NormalMatrix"), 1, GL_FALSE, &normalMatrix[0][0]);
	
	




	glFlush();
	glutSwapBuffers();
}

void reshape(int width, int height)		// Resize the OpenGL window
{
	screenWidth=width; screenHeight = height;           // to ensure the mouse coordinates match 
														// we will use these values to set the coordinate system

	glViewport(0,0,width,height);						// Reset The Current Viewport

	//Set the projection matrix
	ProjectionMatrix = glm::perspective(glm::radians(60.0f), (GLfloat)screenWidth/(GLfloat)screenHeight, 1.0f, 200.0f);
}
void init()
{
	glClearColor(1.0,1.0,1.0,0.0);						//sets the clear colour to yellow
														//glClear(GL_COLOR_BUFFER_BIT) in the display function
														//will clear the buffer to this colour
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);


	myShader = new CShader();
	//if(!myShader->CreateShaderProgram("BasicView", "glslfiles/basicTransformationsWithDisplacement.vert", "glslfiles/basicTransformationsWithDisplacement.frag"))
	if(!myShader->CreateShaderProgram("BasicView", "glslfiles/basicTransformations.vert", "glslfiles/basicTransformations.frag"))
	{
		cout << "failed to load shader" << endl;
	}		

	myBasicShader = new CShader();
	if(!myBasicShader->CreateShaderProgram("Basic", "glslfiles/basic.vert", "glslfiles/basic.frag"))
	{
		cout << "failed to load shader" << endl;
	}		

	glUseProgram(myShader->GetProgramObjID());  // use the shader

	glEnable(GL_TEXTURE_2D);

	//lets initialise our object's rotation transformation 
	//to the identity matrix
	objectRotation = glm::mat4(1.0f);
	
	cout << " loading model " << endl;
	if (objLoader.LoadModel("TestModels/Horse1.obj"))//returns true if the model is loaded
	{
		cout << " model loaded " << endl;
		Horse.ConstructModelFromOBJLoader(objLoader);
		//Horse.CalcCentrePoint();
		//Horse.CentreOnZero();
		Horse.InitVBO(myShader);
	}
	else
	{
		cout << " model failed to load " << endl;
	}
	if (objLoader.LoadModel("TestModels/Horse2.obj"))//returns true if the model is loaded
	{
		cout << " model loaded " << endl;
		Horse2.ConstructModelFromOBJLoader(objLoader);
		
		Horse2.InitVBO(myShader);
	}
	else
	{
		cout << " model failed to load " << endl;
	}
	if (objLoader.LoadModel("TestModels/Horse3.obj"))//returns true if the model is loaded
	{
		cout << " model loaded " << endl;
		Horse3.ConstructModelFromOBJLoader(objLoader);
		
		Horse3.InitVBO(myShader);
	}
	else
	{
		cout << " model failed to load " << endl;
	}
	if (objLoader.LoadModel("TestModels/Horse4.obj"))//returns true if the model is loaded
	{
		cout << " model loaded " << endl;
		Horse4.ConstructModelFromOBJLoader(objLoader);
		
		Horse4.InitVBO(myShader);
	}
	else
	{
		cout << " model failed to load " << endl;
	}

	
	if (objLoader.LoadModel("TestModels/carousel1.obj"))//returns true if the model is loaded
	{
		cout << " model loaded " << endl;
		carousel1.ConstructModelFromOBJLoader(objLoader);
		
		carousel1.InitVBO(myShader);

	}
	else
	{
		cout << " model failed to load " << endl;
	}
	if (objLoader.LoadModel("TestModels/carousel2.obj"))//returns true if the model is loaded
	{
		cout << " model loaded " << endl;
		carousel2.ConstructModelFromOBJLoader(objLoader);
		
		carousel2.InitVBO(myShader);

	}
	else
	{
		cout << " model failed to load " << endl;
	}
	if (objLoader.LoadModel("TestModels/carousel3.obj"))//returns true if the model is loaded
	{
		cout << " model loaded " << endl;
		carousel3.ConstructModelFromOBJLoader(objLoader);
		
		carousel3.InitVBO(myShader);

	}
	else
	{
		cout << " model failed to load " << endl;
	}
	if (objLoader.LoadModel("TestModels/castleFloor.obj"))//returns true if the model is loaded
	{
		cout << " model loaded " << endl;
		grassFloor.ConstructModelFromOBJLoader(objLoader);
		grassFloor.CalcCentrePoint();
		grassFloor.CentreOnZero();
		grassFloor.InitVBO(myShader);
	}
	else
	{
		cout << " model failed to load " << endl;
	}
	if (objLoader.LoadModel("TestModels/tower1.obj"))//returns true if the model is loaded
	{
		cout << " model loaded " << endl;
		tower1.ConstructModelFromOBJLoader(objLoader);
		
		tower1.InitVBO(myShader);
	}
	else
	{
		cout << " model failed to load " << endl;
	}
	if (objLoader.LoadModel("TestModels/tower2.obj"))//returns true if the model is loaded
	{
		cout << " model loaded " << endl;
		tower2.ConstructModelFromOBJLoader(objLoader);
		
		tower2.InitVBO(myShader);
	}
	else
	{
		cout << " model failed to load " << endl;
	}
	if (objLoader.LoadModel("TestModels/tower3.obj"))//returns true if the model is loaded
	{
		cout << " model loaded " << endl;
		tower3.ConstructModelFromOBJLoader(objLoader);
		
		tower3.InitVBO(myShader);
	}
	else
	{
		cout << " model failed to load " << endl;
	}
	if (objLoader.LoadModel("TestModels/tower4.obj"))//returns true if the model is loaded
	{
		cout << " model loaded " << endl;
		tower4.ConstructModelFromOBJLoader(objLoader);
		
		tower4.InitVBO(myShader);
	}
	else
	{
		cout << " model failed to load " << endl;
	}
	if (objLoader.LoadModel("TestModels/castleWall1.obj"))//returns true if the model is loaded
	{
		cout << " model loaded " << endl;
		castleWall1.ConstructModelFromOBJLoader(objLoader);
		
		castleWall1.InitVBO(myShader);
	}
	else
	{
		cout << " model failed to load " << endl;
	}
	if (objLoader.LoadModel("TestModels/castleWall2.obj"))//returns true if the model is loaded
	{
		cout << " model loaded " << endl;
		castleWall2.ConstructModelFromOBJLoader(objLoader);
		
		castleWall2.InitVBO(myShader);
	}
	else
	{
		cout << " model failed to load " << endl;
	}
	if (objLoader.LoadModel("TestModels/castleWall3.obj"))//returns true if the model is loaded
	{
		cout << " model loaded " << endl;
		castleWall3.ConstructModelFromOBJLoader(objLoader);
		
		castleWall3.InitVBO(myShader);
	}
	else
	{
		cout << " model failed to load " << endl;
	}
	if (objLoader.LoadModel("TestModels/castleWall4.obj"))//returns true if the model is loaded
	{
		cout << " model loaded " << endl;
		castleWall4.ConstructModelFromOBJLoader(objLoader);
		
		castleWall4.InitVBO(myShader);
	}
	else
	{
		cout << " model failed to load " << endl;
	}
	

	
	glBindTexture(GL_TEXTURE_2D, 0);
	
}

void special(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_CTRL_L:
		End = true;
		break;
	case GLUT_KEY_LEFT:
		rideSpeed--;
		break;
	case GLUT_KEY_RIGHT:
		rideSpeed++;
		break;
	
	}
}

void specialUp(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_CTRL_L:
		End = false;
		break;

	}
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'a':
		Left = true;
		break;
	case 'd':
		Right = true;
		break;
	case 'w':
		Up = true;
		break;
	case 's':
		Down = true;
		break;
	case ' ':
		Home = true;
		break;
	case 'c':
		cameraType++;
		if (cameraType > 3) {
			cameraType = 0;
		}
		if (cameraType == 3) {
			camPos.x = camPos.x + 5;
		}
		break;

	case 'p':
		if (pause) {
			pause = false;
		}
		else{
			pause = true;
		}
		break;
	case'r':
		if (R == 1.0f) {
			R = 0.0f;
		}
		else {
			R = 1.0f;
		}
		break;
	case'g':
		if (G == 1.0f) {
			G = 0.0f;
		}
		else {
			G = 1.0f;
		}
		break;
	case'b':
		if (B == 1.0f) {
			B = 0.0f;
		}
		else {
			B = 1.0f;
		}
		break;
	case 'o':
		if (disco == false) {
			disco = true;
		}
		else {
			disco = false;
		}
	}
}

void keyboardUp(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'a':
		Left = false;
		break;
	case 'd':
		Right = false;
		break;
	case 'w':
		Up = false;
		break;
	case 's':
		Down = false;
		break;
	case ' ':
		Home = false;
		break;
	}
}
void motion(int x, int y) {

	
	
	if (!firstLoop) {
		if (firstmouse)
		{
			lastX = x;
			lastY = y;
			firstmouse = false;
		}





		yaw -= 0.4f * (400 - x);
		pitch += 0.4f * (300 - y);

		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;

		glm::vec3 direction;
		direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		direction.y = sin(glm::radians(pitch));
		direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		camFront = glm::normalize(direction);
		
	}
	firstLoop = false;
	glutWarpPointer(400, 300);
	

};

void DrawAndCheckCollision(CThreeDModel model) {
	model.DrawElementsUsingVBO(myShader);
	model.CalcBoundingBox(maxX, maxY, maxZ, minX, minY, minZ);
	
	
	if (collision == true) {
		if (!(camPos.x > minX + 1.5 + xoffset) && !(camPos.z > minZ + zoffset) && !(camPos.z < maxZ + zoffset) && !(camPos.y < maxY + yoffset) && !(camPos.y > minY + yoffset) && (abs((camPos.x - minX - xoffset)) < abs((camPos.x - maxX - xoffset)))) {
			camPos.x = minX + 1.5 + xoffset;
		}
		if (!(camPos.x < maxX - 1.5 + xoffset) && !(camPos.z > minZ + zoffset) && !(camPos.z < maxZ + zoffset) && !(camPos.y < maxY + yoffset) && !(camPos.y > minY + yoffset) && (abs((camPos.x - minX - xoffset)) > abs((camPos.x - maxX - xoffset)))) {
			camPos.x = maxX - 1.5 + xoffset;
		}
		if (!(camPos.z > minZ + 1.5 + zoffset) && !(camPos.x > minX + xoffset) && !(camPos.x < maxX + xoffset) && !(camPos.y < maxY + yoffset) && !(camPos.y > minY + yoffset) && (abs((camPos.z - minZ - zoffset)) < abs((camPos.z - maxZ - zoffset)))) {
			camPos.z = minZ + 1.5 + zoffset;
		}
		if (!(camPos.z < maxZ - 1.5 + zoffset) && !(camPos.x > minX + xoffset) && !(camPos.x < maxX + xoffset) && !(camPos.y < maxY + yoffset) && !(camPos.y > minY + yoffset) && (abs((camPos.z - minZ - zoffset)) > abs((camPos.z - maxZ - zoffset)))) {
			camPos.z = maxZ - 1.5 + zoffset;
		}
		if (!(camPos.y > minY + 1.5 + yoffset) && !(camPos.x > minX + xoffset) && !(camPos.x < maxX + xoffset) && !(camPos.z < maxZ + zoffset) && !(camPos.z > minZ + zoffset) && (abs((camPos.y - minY -yoffset)) < abs((camPos.y - maxY - yoffset)))) {
			camPos.y = minY + 1.5 + yoffset;
		}
		if (!(camPos.y < maxY - 1.5 + yoffset) && !(camPos.x > minX + xoffset) && !(camPos.x < maxX + xoffset) && !(camPos.z < maxZ + zoffset) && !(camPos.z > minZ + zoffset) && (abs((camPos.y - minY - yoffset)) > abs((camPos.y - maxY - yoffset)))) {
			camPos.y = maxY - 1.5 + yoffset;
		}
		
	}
	
}

void processKeys()
{
	float spinXinc = 0.0f, spinYinc = 0.0f, spinZinc = 0.0f;
	if (Left)
	{
		camPos -= 0.05f * glm::normalize(glm::cross(camFront, camUp));
	}
	if (Right)
	{
		camPos += 0.05f * glm::normalize(glm::cross(camFront, camUp));
	}
	if (Up)
	{
		camPos += 0.05f * camFront;
	}
	if (Down)
	{
		camPos -= 0.05f * camFront;
	}
	if (Home)
	{
		camPos.y += 0.05f;
	}
	if (End)
	{
		camPos.y -= 0.05f;
	}

}

void idle()
{
	spin += speed;
	if(spin > 360)
		spin = 0;

	processKeys();

	glutPostRedisplay();
}
/**************** END OPENGL FUNCTIONS *************************/

int main(int argc, char **argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(screenWidth, screenHeight);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("OpenGL FreeGLUT Example: Obj loading");

	//This initialises glew - it must be called after the window is created.
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		cout << " GLEW ERROR" << endl;
	}

	
	//Check the OpenGL version being used
	int OpenGLVersion[2];
	glGetIntegerv(GL_MAJOR_VERSION, &OpenGLVersion[0]);
	glGetIntegerv(GL_MINOR_VERSION, &OpenGLVersion[1]);
	cout << OpenGLVersion[0] << " " << OpenGLVersion[1] << endl;


	//initialise the objects for rendering
	init();
	//glutFullScreen();
	glutReshapeFunc(reshape);
	//specify which function will be called to refresh the screen.
	glutDisplayFunc(display);

	glutSpecialFunc(special);
	glutSpecialUpFunc(specialUp);
	glutKeyboardFunc(keyboard);
	glutKeyboardUpFunc(keyboardUp);
	glutPassiveMotionFunc(motion);

	glutIdleFunc(idle);

	//starts the main loop. Program loops and calls callback functions as appropriate.
	glutMainLoop();

	return 0;
}
