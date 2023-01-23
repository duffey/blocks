//PhysX Initialization code adapted from the "Boxes Sample" program contained within the AGEIA PhysX SDK



#include <stdio.h>
#include <iostream>
#include <Windows.h>

#include <glad/gl.h>
#include <GLFW/glfw3.h>


#include "BlockDriver.h"
#include "BlockStructure.h"
#include "Matrix44.h"
#include "Button.h"
#include "NullCommand.h"
#include "ScrollingButtonMenu.h"
#include "Controller.h"
#include "SimulatedModel.h"


// Rendering
static int		gMouseX = 0;
static int		gMouseY = 0;

//My variables
static bool fullscreenEnabled = false;
static double PI = 3.141592654;
static double yRotation = PI / 4.0;
static double xzRotation = 3.0 * PI / 4.0;

static float cameraRadius = 500.0;


enum {X, Y, Z, W};

static GLfloat originalWindowWidth = 640, originalWindowHeight = 360;
static GLfloat windowWidth = originalWindowWidth, windowHeight = originalWindowHeight;

static GLfloat eye[3] = {(cameraRadius*(cos(yRotation)))*cos(xzRotation), cameraRadius*sin(yRotation), (cameraRadius*(cos(yRotation)))*sin(xzRotation)};
static GLint mouse[2] = {0, 0};

Controller* controller;

static bool init()
{
	cout << "Blocks is a game written by Scott Duffey.  For best results, play in full screen mode\n\n";
	cout << "The object of the game is to light up all of the colored blocks without crossing your own path or running into solid blocks.\n\n";
	cout << "Controls:\n\n";
	cout << "F1:\t\t\tToggle full screen\n";
	cout << "F2:\t\t\tToggle debug mode\n";
	cout << "Arrows Keys:\t\tRotate the laser's direction (with respect to itself)\n";
	cout << "w:\t\t\tZoom camera in\n";
	cout << "s:\t\t\tZoom camera out\n";
	cout << "Right-click + Drag:\tRotate camera\n";
	cout << "q:\t\t\tReturn to main menu\n";
	cout << "ESC:\t\t\tQuit\n";
	
	/*if (GLEE_WGL_EXT_swap_control)
	{
		cout << "wgl_ext supported" << endl;
		wglSwapIntervalEXT(1);
	}
	else
		cout << "wgl_ext unsupported" << endl;*/





	controller = new Controller(windowWidth, windowHeight);

	return true;
}

static void exit()
{
	delete controller;
}

static void rotateXZ(double radian)
{
	xzRotation += radian;
	if(xzRotation > 2*PI)
		xzRotation = 0;
	if(xzRotation < 0)
		xzRotation = 2*PI;
	eye[X] = (cameraRadius*(cos(yRotation)))*cos(xzRotation);
	eye[Z] = (cameraRadius*(cos(yRotation)))*sin(xzRotation);
}

static void rotateY(double radian)
{
	rotateXZ(0);
	yRotation += radian;
	if(yRotation > 2*PI)
		yRotation = 0;
	if(yRotation < 0)
		yRotation = 2*PI;
	eye[Y] = cameraRadius*sin(yRotation);
	rotateXZ(0);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		(window, GLFW_TRUE);
	if (action == GLFW_PRESS)
		controller->sendKeyPress(key);
}

int left_action = GLFW_RELEASE;
int right_action = GLFW_RELEASE;

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	int origmousex = mouse[X];
	int origmousey = mouse[Y];
	mouse[X] = (int) xpos;
	mouse[Y] = (int) ypos;

	if (controller == NULL) return;

	if (controller->isMainMenuEnabled()) {
		controller->sendMouseUp(mouse[X], windowHeight - mouse[Y]);
	}
	else {
		if (left_action == GLFW_PRESS) {
			controller->sendMouseDown(mouse[X], windowHeight - mouse[Y]);
		}
		if (right_action == GLFW_PRESS) {
			controller->sendMouseDown(mouse[X], windowHeight - mouse[Y]);
			rotateXZ(-1.0 * (origmousex - mouse[X]) / 100.0);
			rotateY(-1.0 * (origmousey - mouse[Y]) / 100.0);
		}
		else {
			controller->sendMouseUp(mouse[X], windowHeight - mouse[Y]);
		}
	}
}


void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_RIGHT)
		right_action = action;
	else if (button == GLFW_MOUSE_BUTTON_LEFT)
		left_action = action;

	if (left_action == GLFW_PRESS)
		controller->sendMouseDown(mouse[X], windowHeight - mouse[Y]);
	else
		controller->sendMouseUp(mouse[X], windowHeight - mouse[Y]);
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	windowWidth = width;
	windowHeight = height;

	controller->setWindowWidth(width);
	controller->setWindowHeight(height);

	glViewport(0, 0, windowWidth, windowHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, (double)(windowWidth / windowHeight), 0.1, 2000.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	cout << "resize ran" << endl;
}

static void idle() { glutPostRedisplay(); }

void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}


int main(int argc, char** argv)
{
	GLFWwindow* window;

	// Initialize glut
	
	glfwSetErrorCallback(error_callback);

	if (!glfwInit())
        exit(EXIT_FAILURE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}


	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	glfwMakeContextCurrent(window);
	gladLoadGL(glfwGetProcAddress);
	glfwSwapInterval(1);

	init();

	framebuffer_size_callback(NULL, 640, 480);

	while (!glfwWindowShouldClose(window))
	{
		float ratio;
		int width, height;

		glfwGetFramebufferSize(window, &width, &height);
		ratio = width / (float)height;

		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT);

		// Draw stuff
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(eye[X], eye[Y], eye[Z], 0.0, 95.0, 0.0, 0, 1, 0);
		//gluLookAt(0.0, 0, -1.0, 0.0, 0.0, 0.0, 0, 1, 0);

		controller->display();
		//glutSolidTeapot(1.0);

		glFinish();

		controller->update();

		glFinish();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);

	glfwTerminate();
	exit(EXIT_SUCCESS);
	
	/*
	glutInit(&argc, argv);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(originalWindowWidth, originalWindowHeight);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STENCIL);
	int mainHandle = glutCreateWindow("Blocks");
	glutSetWindow(mainHandle);
	glutDisplayFunc(display);
	glutReshapeFunc(resize);
	glutIdleFunc(idle);
	glutKeyboardFunc(keyPress);
	glutSpecialFunc(arrowKeyPress);
	glutMouseFunc(mousePress);
	glutMotionFunc(mouseDrag);
	glutPassiveMotionFunc(mouseMove);
	mouseDrag(0,0);
	atexit(exit);

	// Initialize physics scene and start the application main loop if scene was created
	if (init())
		glutMainLoop();

	glfwTerminate();
	*/
	return 0;
}
