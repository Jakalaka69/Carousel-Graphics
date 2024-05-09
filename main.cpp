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

CShader* myShader;  ///shader object 
CShader* myBasicShader;

//MODEL LOADING
#include "3DStruct\threeDModel.h"
#include "Obj\OBJLoader.h"

float amount = 0;
float temp = 0.002f;
int cameraType = 0;

CThreeDModel Horse,Horse2,Horse3,Horse4,grassFloor,carouselFloor,Terrain; //A threeDModel object is needed for each model loaded
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
glm::vec3 camPos = glm::vec3(10.0f, 0.0f, 0.0f);
glm::vec3 camFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 camUp = glm::vec3(0.0f, 1.0f, 0.0f);

float horseAngle = 0.0f;
float bounceAngle = 0.0f;
float rotateAngle = PI;
float camRight = 0.0;
float camFoward = 0.0;
float lightAngle = 0.f;
float lastCamX = 0.0f;
float lastCamY = 0.0f;
float lastCamZ = 0.0f;

//Material properties
float Material_Ambient[4] = {1.0f, 1.0f, 1.0f, 1.0f};
float Material_Diffuse[4] = {0.7f, 0.7f, 0.7f, 1.0f};
float Material_Specular[4] = {1.0f,1.0f,1.0f,1.0f};
float Material_Shininess = 50;

//Light Properties
float Light_Ambient[4] = {1.0f, 1.0f, 1.0f, 1.0f};
float Light_Diffuse[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
float Light_Specular[4] = { 1.0f,1.0f,1.0f,1.0f};
float LightPos[4] = {0.0f, 1.0f, 0.0f, 0.0f};

//
int	mouse_x=0, mouse_y=0;
bool LeftPressed = false;
int screenWidth=1920, screenHeight=1080;
float lastX = 400, lastY = 300;
float yaw = -90.0f;
float pitch = 0.0f;



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

/*************    START OF OPENGL FUNCTIONS   ****************/
void display()									
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glUseProgram(myShader->GetProgramObjID());  // use the shader

	//Part for displacement shader.
	amount += temp;
	if(amount > 1.0f || amount < -1.5f)
		temp = -temp;
	//amount = 0;
	glUniform1f(glGetUniformLocation(myShader->GetProgramObjID(), "displacement"), amount);

	//Set the projection matrix in the shader
	GLuint projMatLocation = glGetUniformLocation(myShader->GetProgramObjID(), "ProjectionMatrix");  
	glUniformMatrix4fv(projMatLocation, 1, GL_FALSE, &ProjectionMatrix[0][0]);


	//----------------------------------------------------------------------
	rotateAngle += 0.005f;
	if (rotateAngle > 360.0)
		rotateAngle = 0;

	bounceAngle += 0.01f;
	if (bounceAngle > 360) {
		bounceAngle = 0;
	}

	horseAngle += 0.005f;
	if (horseAngle > 360.0)
		horseAngle = 0;

	pos.y = 0.5 * sin(bounceAngle) -4.5;
	pos2.y = 0.5 * sin(bounceAngle - 60)-4.5;
	pos3.y = 0.5 * sin(bounceAngle - 300)-4.5;
	pos4.y = 0.5 * sin(bounceAngle - 180)-4.5;
	

	glm::mat4 viewingMatrix = glm::mat4(1.0f);
	
	if(firstLoop == true){
		viewingMatrix = glm::lookAt(glm::vec3(10.0f,0.0f,0.0f), pos, camUp);
	}
	
	if (cameraType == 0) {
		viewingMatrix = glm::lookAt(camPos, camPos + camFront, camUp);
	}
	else if (cameraType == 1) {
		
		camPos.x = 3.5 * sin(rotateAngle-41);
		camPos.z = 3.5 * cos(rotateAngle-41);
		camPos.y = pos.y + 3.6;
		viewingMatrix = glm::lookAt(camPos, camPos + camFront, camUp);

	}

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
	Horse.DrawBoundingBox(myShader);
	Horse.CalcBoundingBox(maxX, maxY, maxZ, minX, minY, minZ);
	
	
	
	
	
	

	if(!(camPos.x > minX) && !(camPos.x < maxX)) {
		
		if (lastCamX > camPos.x) {
			camPos.x = camPos.x + 0.1;
		}
		else if (lastCamX < camPos.x) {
			camPos.x = camPos.x - 0.1;
		}
	}

	



	lastCamX = camPos.x;

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
	
	carouselFloor.DrawElementsUsingVBO(myShader);
	Terrain.DrawElementsUsingVBO(myShader);

	ModelViewMatrix = glm::translate(viewingMatrix, glm::vec3(0, -5.2, 0));
	glUniformMatrix4fv(glGetUniformLocation(myShader->GetProgramObjID(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);
	normalMatrix = glm::inverseTranspose(glm::mat3(ModelViewMatrix));
	glUniformMatrix3fv(glGetUniformLocation(myShader->GetProgramObjID(), "NormalMatrix"), 1, GL_FALSE, &normalMatrix[0][0]);
	
	
	grassFloor.DrawElementsUsingVBO(myShader);
	
	

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

	
	if (objLoader.LoadModel("TestModels/carousel.obj"))//returns true if the model is loaded
	{
		cout << " model loaded " << endl;
		carouselFloor.ConstructModelFromOBJLoader(objLoader);
		carouselFloor.CalcCentrePoint();
		carouselFloor.CentreOnZero();
		carouselFloor.InitVBO(myShader);

	}
	else
	{
		cout << " model failed to load " << endl;
	}
	if (objLoader.LoadModel("TestModels/grassFloor.obj"))//returns true if the model is loaded
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
	

	
	glBindTexture(GL_TEXTURE_2D, 0);
	
}

void special(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_CTRL_L:
		End = true;
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
		if (cameraType > 1) {
			cameraType = 0;
		}
		break;
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





		yaw -= 0.4f * (960 - x);
		pitch += 0.4f * (540 - y);

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
	glutWarpPointer(960, 540);
	

};

void processKeys()
{
	float spinXinc = 0.0f, spinYinc = 0.0f, spinZinc = 0.0f;
	if (Left)
	{
		camPos -= 0.1f * glm::normalize(glm::cross(camFront, camUp));
	}
	if (Right)
	{
		camPos += 0.1f * glm::normalize(glm::cross(camFront, camUp));
	}
	if (Up)
	{
		camPos += 0.1f * camFront;
	}
	if (Down)
	{
		camPos -= 0.1f * camFront;
	}
	if (Home)
	{
		camPos.y += 0.1f;
	}
	if (End)
	{
		camPos.y -= 0.1f;
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
	glutInitWindowPosition(0, 0);
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
	glutFullScreen();
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
