#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#include <string>	//string and rfind
#include <iostream>	//cout
#include <memory>	//auto_ptr
#include <cassert>	//assert

#include "Vector4.h"

using namespace std;

bool pathset;
string path;

/**
  * @brief This class is a part of the implementation of the %Model View %Controller pattern
  * This is the controller which drives the main operation of the program (i.e. switching views when necessary, taking in mouse and keyboard input, etc.).
  */
class Controller
{
	private:
		static const GLfloat paddingRatio;
		static const GLfloat blockSize;
		static const Vector4 base;

		enum { x, y, z, w };

		static const GLfloat groundPlaneSize;
		static const double PI;
		static const double angle;
		static const double angle2;
		static const float radius;

		//Point Light
		static const GLfloat light0Position[4];
		static const GLfloat light0SpecularIntensity[4];
		static const GLfloat light0DiffuseIntensity[4];
		static const GLfloat light0AmbientIntensity[4];

		//Spot Light
		static const GLfloat light1Position[4];
		static const GLfloat light1Direction[4];
		static const GLfloat light1SpecularIntensity[4];
		static const GLfloat light1DiffuseIntensity[4];
		static const GLfloat light1AmbientIntensity[4];

		//Dim Point Light (at same position as first point light)
		static const GLfloat light2Position[4];
		static const GLfloat light2SpecularIntensity[4];
		static const GLfloat light2DiffuseIntensity[4];
		static const GLfloat light2AmbientIntensity[4];

		static const GLfloat Material1Specular[4];
		static const GLfloat Material1Diffuse[4];
		static const GLfloat Material1Ambient[4];
		static const GLfloat Material1Shininess;

		static const GLfloat Material2Specular[4];
		static const GLfloat Material2Diffuse[4];
		static const GLfloat Material2Ambient[4];
		static const GLfloat Material2Shininess;




		bool mainMenuEnabled, debugViewEnabled;
		GLfloat originalWindowWidth, originalWindowHeight, currentWindowWidth, currentWindowHeight;
		BlockDriver blockDriver;
		auto_ptr<BlockDriver::Laser> laser;
		const BlockStructure* blockStructure;
		GLuint groundTexture;

	public:
		Controller(	const GLfloat& windowWidth,
					const GLfloat& windowHeight) :
		
					mainMenuEnabled(true),
					debugViewEnabled(false),

					originalWindowWidth(windowWidth), originalWindowHeight(windowHeight),
					currentWindowWidth(windowWidth), currentWindowHeight(windowHeight),

					groundTexture(loadRawTexture("textures/psycho2.raw", 1))
		{

				//assert(fs::exists(blockStructurePath) && fs::is_directory(blockStructurePath));


				WIN32_FIND_DATA ffd;
				HANDLE hFind = INVALID_HANDLE_VALUE;
				LARGE_INTEGER filesize;
				hFind = FindFirstFile(L"puzzles\\*", &ffd);




				blockStructure = blockDriver.getBlockStructure();

		}

		//Credit: http://www.nullterminator.net/gltexture.html
		static GLuint loadRawTexture(const char* filename, int wrap)
		{
			GLuint texture;
			int width, height;
			void* data;
			FILE* file;

			// open texture data
			file = fopen(filename, "rb");
			if (file == NULL) return 0;

			// allocate buffer
			width = 512;
			height = 512;
			data = malloc(width * height * 3);

			// read texture data
			fread(data, width * height * 3, 1, file);
			fclose(file);

			// allocate a texture name
			glGenTextures(1, &texture);

			// select our current texture
			glBindTexture(GL_TEXTURE_2D, texture);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

			// free buffer
			free(data);

			return texture;
		}

		void sendKeyPress(int key)
		{

			switch(key)
			{
				case GLFW_KEY_Q:				if(blockDriver.isLoaded())
										{
		

											mainMenuEnabled = true;
											laser.reset();
											blockDriver.unload();
										}
										break;

				case GLFW_KEY_F2:		debugViewEnabled = !debugViewEnabled;
					
										break;

				case GLFW_KEY_P:		if(laser.get() != 0 && blockDriver.isLoaded()) laser -> setMobile(!laser -> isMobile());
										break;

				case GLFW_KEY_UP:		if(laser.get() != 0 && blockDriver.isLoaded()) laser -> turn(BlockDriver::Laser::UP);
										break;

				case GLFW_KEY_DOWN:		if(laser.get() != 0 && blockDriver.isLoaded()) laser -> turn(BlockDriver::Laser::DOWN);
										break;

				case GLFW_KEY_LEFT:		if(laser.get() != 0 && blockDriver.isLoaded()) laser -> turn(BlockDriver::Laser::LEFT);
										break;

				case GLFW_KEY_RIGHT:	if(laser.get() != 0 && blockDriver.isLoaded()) laser -> turn(BlockDriver::Laser::RIGHT);
										break;
			}
		}

		void update()
		{
			if (pathset) {
				pathset = false;
				blockDriver.reset();
				blockDriver.loadBlockStructure(new BlockStructure(path, 30.0, Vector4(0.0, 50.0, 0.0, 1.0)));
			}

			if(blockDriver.isLoaded())
			{
				//We already have a game in progress.
				if(laser.get() != 0)
				{
					if(laser -> isMobile())
					{
						laser -> move();
					}
					//Return to the main menu if we are not in debug mode
					else if(!debugViewEnabled && laser -> isIdle())
					{


						mainMenuEnabled = true;
						laser.reset();
						blockDriver.unload();
					}
				}
				//We must have just loaded a block structure, so let's get a new laser.
				else
				{
					laser.reset();
					laser = blockDriver.getLaser();
					mainMenuEnabled = false;

				}
			}
		}

		void draw_menu() {
			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glEnable(GL_DEPTH_BUFFER_BIT);
			glDisable(GL_LIGHTING);

			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			glLoadIdentity();

			glMatrixMode(GL_PROJECTION);
			glPushMatrix();

			glLoadIdentity();
			glOrtho(0, getWindowWidth(), 0, getWindowHeight(), -1, 1);
			glDisable(GL_LIGHTING);
			glDisable(GL_DEPTH_TEST);
			glDepthFunc(GL_ALWAYS);
			glEnable(GL_BLEND);

			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);



			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();


			static int counter = 0;

			ImGui::Begin("Levels");                          // Create a window called "Hello, world!" and append into it.


			WIN32_FIND_DATA ffd;
			HANDLE hFind = INVALID_HANDLE_VALUE;
			LARGE_INTEGER filesize;
			hFind = FindFirstFile(L"puzzles\\*", &ffd);


			do
			{
				if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
				}
				else {
					filesize.LowPart = ffd.nFileSizeLow;
					filesize.HighPart = ffd.nFileSizeHigh;

					wstring ws(ffd.cFileName);
					string str(ws.begin(), ws.end());
					size_t dot = str.find_last_of(".");
					string nameWithoutExt = str.substr(0, dot);



					if (ImGui::Button(nameWithoutExt.c_str())) {
						pathset = true;
						path = string("puzzles/") + str;
					}

				}


			} while (FindNextFile(hFind, &ffd) != 0);


			ImGui::End();


			ImGui::Render();
			int display_w, display_h;
			//glfwGetFramebufferSize(window, &display_w, &display_h);
			glViewport(0, 0, 640, 480);
			ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
			glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
			glClear(GL_COLOR_BUFFER_BIT);
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());



			glDisable(GL_BLEND);
			glDepthFunc(GL_LEQUAL);
			glEnable(GL_DEPTH_TEST);
			glEnable(GL_LIGHTING);

			glPopMatrix();

			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();
		}

		void display()
		{
			if (mainMenuEnabled) {
				draw_menu();
			}
			else {
				blockStructure = blockDriver.getBlockStructure();
				if (debugViewEnabled) {
					display_debug();
				}
				else {
					blocks_display();
				}
			}

		}



		void blocks_display()
		{

			glClearDepth(1.0f);
			glDepthFunc(GL_LEQUAL);
			glEnable(GL_DEPTH_TEST);
			glEnable(GL_STENCIL_TEST);

			glShadeModel(GL_SMOOTH);
			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

			glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
			glEnable(GL_COLOR_MATERIAL);
			glEnable(GL_NORMALIZE);
			glEnable(GL_CULL_FACE);

			glLightfv(GL_LIGHT0, GL_POSITION, light0Position);
			glLightfv(GL_LIGHT0, GL_SPECULAR, light0SpecularIntensity);
			glLightfv(GL_LIGHT0, GL_DIFFUSE, light0DiffuseIntensity);
			glLightfv(GL_LIGHT0, GL_AMBIENT, light0AmbientIntensity);

			glLightfv(GL_LIGHT1, GL_POSITION, light1Position);
			glLightfv(GL_LIGHT1, GL_SPECULAR, light1SpecularIntensity);
			glLightfv(GL_LIGHT1, GL_DIFFUSE, light1DiffuseIntensity);
			glLightfv(GL_LIGHT1, GL_AMBIENT, light1AmbientIntensity);
			glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 1.0f);
			glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 1.0f);
			glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.0f);
			glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.0f);
			glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, light1Direction);
			glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 90.0);

			glLightfv(GL_LIGHT2, GL_SPECULAR, light2SpecularIntensity);
			glLightfv(GL_LIGHT2, GL_DIFFUSE, light2DiffuseIntensity);
			glLightfv(GL_LIGHT2, GL_AMBIENT, light2AmbientIntensity);


			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
			glClearStencil(0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			glEnable(GL_DEPTH_BUFFER_BIT);
			glEnable(GL_STENCIL_BUFFER_BIT);
			glEnable(GL_LIGHTING);
			glEnable(GL_LIGHT0);
			glDisable(GL_LIGHT1);
			glEnable(GL_LIGHT2);

			glMatrixMode(GL_MODELVIEW);


			if (blockStructure != NULL)
			{
				glPushAttrib(GL_DEPTH_BUFFER_BIT | GL_LIGHTING_BIT | GL_STENCIL_BUFFER_BIT);

				glDisable(GL_LIGHT0);

				drawScene();

				glEnable(GL_LIGHT0);

				glPushAttrib(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_POLYGON_BIT | GL_STENCIL_BUFFER_BIT);

				glColorMask(0, 0, 0, 0);
				glDepthMask(0);
				glEnable(GL_CULL_FACE);
				glEnable(GL_STENCIL_TEST);

				glStencilFunc(GL_ALWAYS, 0, ~0);
				glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
				glCullFace(GL_BACK);

				blockStructure->drawShadowVolume(Vector4(light0Position[x], light0Position[y], light0Position[z], light0Position[w]));

				glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);
				glCullFace(GL_FRONT);

				blockStructure->drawShadowVolume(Vector4(light0Position[x], light0Position[y], light0Position[z], light0Position[w]));

				glPopAttrib();

				glDepthFunc(GL_EQUAL);
				glEnable(GL_STENCIL_TEST);
				glStencilFunc(GL_EQUAL, 0, ~0);
				glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);


				drawScene();

				glPopAttrib();
			}
			else
				drawScene();
		}

		void drawScene()
		{
			glMatrixMode(GL_MODELVIEW);

			//Draw the block structure
			if (blockStructure != NULL)
			{
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				glMaterialfv(GL_FRONT, GL_SPECULAR, Material1Specular);
				glMaterialfv(GL_FRONT, GL_DIFFUSE, Material1Diffuse);
				glMaterialfv(GL_FRONT, GL_AMBIENT, Material1Ambient);
				glMaterialf(GL_FRONT, GL_SHININESS, Material1Shininess);

				blockStructure->draw();

				glDisable(GL_BLEND);
			}

			//Draw the laser
			glDisable(GL_LIGHTING);

			laser->draw();

			glEnable(GL_LIGHTING);

			//Draw the ground
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, groundTexture);

			glMaterialfv(GL_FRONT, GL_SPECULAR, Material2Specular);
			glMaterialfv(GL_FRONT, GL_DIFFUSE, Material2Diffuse);
			glMaterialfv(GL_FRONT, GL_AMBIENT, Material2Ambient);
			glMaterialf(GL_FRONT, GL_SHININESS, Material2Shininess);
			glColor3f(1.0, 1.0, 1.0);

			glBegin(GL_QUADS);

			glNormal3f(0.0, 1.0, 0.0);
			glTexCoord2f(0.0, 0.0);	glVertex3f(-groundPlaneSize, 0.0, groundPlaneSize);
			glTexCoord2f(1.0, 0.0); glVertex3f(groundPlaneSize, 0.0, groundPlaneSize);
			glTexCoord2f(1.0, 1.0); glVertex3f(groundPlaneSize, 0.0, -groundPlaneSize);
			glTexCoord2f(0.0, 1.0); glVertex3f(-groundPlaneSize, 0.0, -groundPlaneSize);

			glEnd();

			glDisable(GL_TEXTURE_2D);
		}



		virtual void display_debug() const
		{
			glClearDepth(1.0f);
			glDepthFunc(GL_LEQUAL);
			glEnable(GL_DEPTH_TEST);
			glEnable(GL_STENCIL_TEST);

			glShadeModel(GL_SMOOTH);
			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

			glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
			glEnable(GL_COLOR_MATERIAL);
			glEnable(GL_NORMALIZE);
			glEnable(GL_CULL_FACE);

			glLightfv(GL_LIGHT0, GL_POSITION, light0Position);
			glLightfv(GL_LIGHT0, GL_SPECULAR, light0SpecularIntensity);
			glLightfv(GL_LIGHT0, GL_DIFFUSE, light0DiffuseIntensity);
			glLightfv(GL_LIGHT0, GL_AMBIENT, light0AmbientIntensity);

			glLightfv(GL_LIGHT1, GL_POSITION, light1Position);
			glLightfv(GL_LIGHT1, GL_SPECULAR, light1SpecularIntensity);
			glLightfv(GL_LIGHT1, GL_DIFFUSE, light1DiffuseIntensity);
			glLightfv(GL_LIGHT1, GL_AMBIENT, light1AmbientIntensity);
			glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 1.0f);
			glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 1.0f);
			glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.0f);
			glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.0f);
			glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, light1Direction);
			glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 90.0);

			glLightfv(GL_LIGHT2, GL_SPECULAR, light2SpecularIntensity);
			glLightfv(GL_LIGHT2, GL_DIFFUSE, light2DiffuseIntensity);
			glLightfv(GL_LIGHT2, GL_AMBIENT, light2AmbientIntensity);


			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
			glClearStencil(0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			glEnable(GL_DEPTH_BUFFER_BIT);
			glEnable(GL_STENCIL_BUFFER_BIT);
			glEnable(GL_LIGHTING);
			glEnable(GL_LIGHT0);
			glDisable(GL_LIGHT1);
			glEnable(GL_LIGHT2);

			glMatrixMode(GL_MODELVIEW);

			if (blockStructure != NULL)
			{
				glPushAttrib(GL_DEPTH_BUFFER_BIT | GL_LIGHTING_BIT | GL_STENCIL_BUFFER_BIT);

				glDisable(GL_LIGHT0);

				drawScene_debug();

				glEnable(GL_LIGHT0);

				glPushAttrib(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_POLYGON_BIT | GL_STENCIL_BUFFER_BIT);

				glColorMask(0, 0, 0, 0);
				glDepthMask(0);
				glEnable(GL_CULL_FACE);
				glEnable(GL_STENCIL_TEST);

				glStencilFunc(GL_ALWAYS, 0, ~0);
				glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
				glCullFace(GL_BACK);

				blockStructure->drawShadowVolume(Vector4(light0Position[x], light0Position[y], light0Position[z], light0Position[w]));

				glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);
				glCullFace(GL_FRONT);

				blockStructure->drawShadowVolume(Vector4(light0Position[x], light0Position[y], light0Position[z], light0Position[w]));

				glPopAttrib();

				glDepthFunc(GL_EQUAL);
				glEnable(GL_STENCIL_TEST);
				glStencilFunc(GL_EQUAL, 0, ~0);
				glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

				drawScene_debug();

				glPopAttrib();
			}
			else
				drawScene_debug();
		}

		void drawScene_debug() const
		{
			glMatrixMode(GL_MODELVIEW);

			//Draw the block structure
			if (blockStructure != NULL)
			{
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				glMaterialfv(GL_FRONT, GL_SPECULAR, Material1Specular);
				glMaterialfv(GL_FRONT, GL_DIFFUSE, Material1Diffuse);
				glMaterialfv(GL_FRONT, GL_AMBIENT, Material1Ambient);
				glMaterialf(GL_FRONT, GL_SHININESS, Material1Shininess);

				//blockStructure->draw();

				glDisable(GL_LIGHTING);

				//blockStructure->drawShadowVolume(Vector4(light0Position[x], light0Position[y], light0Position[z], light0Position[w]));

				glEnable(GL_LIGHTING);

				glDisable(GL_BLEND);

				glLineWidth(1.0);
				glDisable(GL_LIGHTING);

				blockDriver.drawVoxelGrid();
				blockDriver.highlightVoxel(1, 1, 1);
				blockDriver.showVoxelTurnThreshold(2, 2, 2);

				glEnable(GL_LIGHTING);
			}

			//Draw the laser
			glDisable(GL_LIGHTING);

			laser->draw();

			glEnable(GL_LIGHTING);

			//Draw the ground
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, groundTexture);

			glMaterialfv(GL_FRONT, GL_SPECULAR, Material2Specular);
			glMaterialfv(GL_FRONT, GL_DIFFUSE, Material2Diffuse);
			glMaterialfv(GL_FRONT, GL_AMBIENT, Material2Ambient);
			glMaterialf(GL_FRONT, GL_SHININESS, Material2Shininess);
			glColor3f(1.0, 1.0, 1.0);

			glBegin(GL_QUADS);

			glNormal3f(0.0, 1.0, 0.0);
			glTexCoord2f(0.0, 0.0);	glVertex3f(-groundPlaneSize, 0.0, groundPlaneSize);
			glTexCoord2f(1.0, 0.0); glVertex3f(groundPlaneSize, 0.0, groundPlaneSize);
			glTexCoord2f(1.0, 1.0); glVertex3f(groundPlaneSize, 0.0, -groundPlaneSize);
			glTexCoord2f(0.0, 1.0); glVertex3f(-groundPlaneSize, 0.0, -groundPlaneSize);

			glEnd();

			glDisable(GL_TEXTURE_2D);
		}




		void sendMouseUp(GLfloat mouseX, GLfloat mouseY)
		{
		}
		
		void sendMouseDown(GLfloat mouseX, GLfloat mouseY)
		{
		}

		void setWindowWidth(const GLfloat& windowWidth) { currentWindowWidth = windowWidth; }

		void setWindowHeight(const GLfloat& windowHeight) { currentWindowHeight = windowHeight; }

		const GLfloat& getWindowWidth() const { return currentWindowWidth; }

		const GLfloat& getWindowHeight() const { return currentWindowHeight; }

		bool isMainMenuEnabled() const { return mainMenuEnabled; }
};

const GLfloat Controller::paddingRatio = .02;
const GLfloat Controller::blockSize = 30.0;
const Vector4 Controller::base = Vector4(0.0, 50.0, 0.0, 1.0);

const GLfloat Controller::groundPlaneSize = 200.0;
const double Controller::PI = 3.141592654;
const double Controller::angle = PI / 4.0;
const double Controller::angle2 = 5.0 * PI / 4.0;
const float Controller::radius = 300.0;

const GLfloat Controller::light0Position[4] = { radius * cos(angle), 600.0, radius * sin(angle), 1.0f };
const GLfloat Controller::light0SpecularIntensity[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat Controller::light0DiffuseIntensity[4] = { 1.0, 1.0, 1.0, 1.0 };
const GLfloat Controller::light0AmbientIntensity[4] = { 0.2f, 0.2f, 0.2f, 1.0f };

//Spot Light
const GLfloat Controller::light1Position[4] = { radius * cos(angle2), 600.0, radius * sin(angle2), 1.0f };
const GLfloat Controller::light1Direction[4] = { 0.0, -1.0, 0.0, 0.0f };
const GLfloat Controller::light1SpecularIntensity[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat Controller::light1DiffuseIntensity[4] = { 1.0, 1.0, 1.0, 1.0 };
const GLfloat Controller::light1AmbientIntensity[4] = { 0.2f, 0.2f, 0.2f, 1.0f };

//Dim Point Light (at same position as first point light)
const GLfloat Controller::light2Position[4] = { radius * cos(angle), 600.0, radius * sin(angle), 1.0f };
const GLfloat Controller::light2SpecularIntensity[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
const GLfloat Controller::light2DiffuseIntensity[4] = { 0.2, 0.2, 0.2, 1.0 };
const GLfloat Controller::light2AmbientIntensity[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

const GLfloat Controller::Material1Specular[4] = { 1.0, 1.0, 1.0, 1.0 };
const GLfloat Controller::Material1Diffuse[4] = { 0.2, 0.2, 0.2, 1.0 };
const GLfloat Controller::Material1Ambient[4] = { 0.2, 0.2, 0.2, 1.0 };
const GLfloat Controller::Material1Shininess = 70.0;

const GLfloat Controller::Material2Specular[4] = { 0.2, 0.2, 0.2, 1.0 };
const GLfloat Controller::Material2Diffuse[4] = { 1.0, 1.0, 1.0, 1.0 };
const GLfloat Controller::Material2Ambient[4] = { 0.2, 0.2, 0.2, 1.0 };
const GLfloat Controller::Material2Shininess = 10.0;

#endif /*CONTROLLER_H_*/
