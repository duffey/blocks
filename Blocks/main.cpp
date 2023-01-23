//PhysX Initialization code adapted from the "Boxes Sample" program contained within the AGEIA PhysX SDK



#include <stdio.h>
#include <iostream>

#define NOMINMAX
#include  <GLee/GLee.h>



#if __APPLE__
#include <GLUT/glut.h>
#else 
#include <GL/glut.h>
#endif


#include "NxPhysics.h"
#include "BlockDriver.h"
#include "BlockStructure.h"
#include "Matrix44.h"
#include "Button.h"
#include "NullCommand.h"
#include "ScrollingButtonMenu.h"
#include "Controller.h"
#include "SimulatedModel.h"

// Physics
static NxPhysicsSDK*	gPhysicsSDK = NULL;
static NxScene*			gScene = NULL;

// Rendering
// Physx
static NxVec3	gEye(50.0f, 50.0f, 50.0f);
static NxVec3	gDir(-0.6f,-0.2f,-0.7f);
static NxVec3	gViewY;
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
static GLint buttons[5] = {GLUT_UP, GLUT_UP, GLUT_UP, GLUT_UP, GLUT_UP};

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

	// Initialize PhysicsSDK
	NxPhysicsSDKDesc desc;
	gPhysicsSDK = NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION);
	if(gPhysicsSDK == NULL) 
	{
		printf("nError: Unable to initialize the PhysX SDK.\n\n");
		return false;
	}

	gPhysicsSDK->setParameter(NX_SKIN_WIDTH, 0.05f);

	// Create a scene
	NxSceneDesc sceneDesc;
	sceneDesc.gravity				= NxVec3(0.0f, -9.81f, 0.0f);
	gScene = gPhysicsSDK->createScene(sceneDesc);
	if(gScene == NULL)
	{
		printf("\nError: Unable to create a PhysX scene.\n\n");
		return false;
	}

	// Set default material
	NxMaterial* defaultMaterial = gScene->getMaterialFromIndex(0);
	defaultMaterial->setRestitution(0.0f);
	defaultMaterial->setStaticFriction(0.5f);
	defaultMaterial->setDynamicFriction(0.5f);

	// Create ground plane
	NxPlaneShapeDesc planeDesc;
	NxActorDesc actorDesc;
	actorDesc.shapes.pushBack(&planeDesc);

	gScene->createActor(actorDesc);

	controller = new Controller(windowWidth, windowHeight, *gScene);

	return true;
}

static void exit()
{
	delete controller;

	if(gPhysicsSDK != NULL)
	{
		if(gScene != NULL) gPhysicsSDK->releaseScene(*gScene);
		gScene = NULL;
		NxReleasePhysicsSDK(gPhysicsSDK);
		gPhysicsSDK = NULL;
	}
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

static void keyPress(unsigned char key, int x, int y)
{
	controller -> sendKeyPress(key);

	switch(key)
	{
		case 27:			exit(0); 
							break;

		case 'w':			cameraRadius -= 8.0;
							rotateXZ(0);
							rotateY(0);
							break;

		case GLUT_KEY_F1:	fullscreenEnabled = !fullscreenEnabled;

							if(fullscreenEnabled)
								glutFullScreen();
							else
							{
								glutPositionWindow(100, 100);
								glutReshapeWindow(originalWindowWidth, originalWindowHeight);
							}
							break;

		case 's':			cameraRadius += 8.0;
							rotateXZ(0);
							rotateY(0);
							break;
	}
}

static void arrowKeyPress(int key, int x, int y) { keyPress(key, x, y); }

static void mousePress(int button, int state, int x, int y)
{
	buttons[button] = state;
	mouse[X] = x;
	mouse[Y] = y;
	
	if(buttons[GLUT_LEFT_BUTTON] == GLUT_DOWN)
		controller -> sendMouseDown(x, windowHeight - y);
	else
		controller -> sendMouseUp(x, windowHeight - y);
}

static void mouseDrag(int x, int y)
{
	if(controller != NULL && !controller -> isMainMenuEnabled())
	{
		if(buttons[GLUT_RIGHT_BUTTON] == GLUT_DOWN)
		{
			rotateXZ(-1.0 * (mouse[X] - x) / 100.0);
			rotateY(-1.0 * (mouse[Y] - y) / 100.0);
		}
		
		if(buttons[GLUT_LEFT_BUTTON] == GLUT_DOWN)
			controller -> sendMouseDown(x, windowHeight - y);
	}

	mouse[X] = x;
	mouse[Y] = y;
}

static void mouseMove(int x, int y)
{
	mouse[X] = x;
	mouse[Y] = y;
	
	controller -> sendMouseUp(x, windowHeight - y);
}

static void display()
{
	if(gScene == NULL) return;
	
	// Start simulation (non blocking)
	gScene->simulate(1.0);

	glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
	gluLookAt(eye[X], eye[Y], eye[Z], 0.0, 95.0, 0.0, 0, 1, 0);

	controller -> display();

	// Fetch simulation results
	gScene->flushStream();
	gScene->fetchResults(NX_RIGID_BODY_FINISHED, true);

	glFinish();

	controller -> update();

	glFinish();

	glutSwapBuffers();
}

static void resize(int width, int height)
{
	windowWidth = width;
	windowHeight = height;

	controller -> setWindowWidth(width);
	controller -> setWindowHeight(height);
	
    glViewport(0, 0, windowWidth, windowHeight);
    glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
    gluPerspective(45.0, (double)(windowWidth / windowHeight), 0.1, 2000.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

static void idle() { glutPostRedisplay(); }

int main(int argc, char** argv)
{
	// Initialize glut
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

	return 0;
}
