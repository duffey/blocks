#ifndef BLOCKSVIEW_H_
#define BLOCKSVIEW_H_

#include "AbstractView.h"
#include "BlockDriver.h"
#include "DrawObjects.h"

/**
  * @brief This class is a part of the implementation of the %Model View %Controller pattern
  * This is the basic View for displaying the gameplay.
  */
class BlocksView : public AbstractView
{
	private:
		enum {x, y, z, w};

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

		//NOTE: Need to change the BlockDriver interface and make this const!
		BlockDriver& blockDriver;
		BlockDriver::Laser& laser;
		const BlockStructure* blockStructure;
		GLuint groundTexture;

	public:
		BlocksView(	const GLfloat& windowWidth,
					const GLfloat& windowHeight,
					BlockDriver& blockDriver,
					BlockDriver::Laser& laser) :
  
					AbstractView(windowWidth, windowHeight),
					blockDriver(blockDriver),
					laser(laser),
					groundTexture(DrawObjects::loadRawTexture("textures/psycho2.raw", 1))
		{
			blockStructure = blockDriver.getBlockStructure();

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
		}

		~BlocksView()
		{
			DrawObjects::freeTexture(groundTexture);
		}

		virtual void display() const
		{
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

			if(blockStructure != NULL)
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

						blockStructure -> drawShadowVolume(Vector4(light0Position[x], light0Position[y], light0Position[z], light0Position[w]));

						glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);
						glCullFace(GL_FRONT);
							
						blockStructure -> drawShadowVolume(Vector4(light0Position[x], light0Position[y], light0Position[z], light0Position[w]));

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

		void drawScene() const
		{
			glMatrixMode(GL_MODELVIEW);

			//Draw the block structure
			if(blockStructure != NULL)
			{
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				glMaterialfv(GL_FRONT, GL_SPECULAR, Material1Specular);
				glMaterialfv(GL_FRONT, GL_DIFFUSE, Material1Diffuse);
				glMaterialfv(GL_FRONT, GL_AMBIENT, Material1Ambient);
				glMaterialf(GL_FRONT, GL_SHININESS, Material1Shininess);
					
					blockStructure -> draw();

				glDisable(GL_BLEND);
			}

			//Draw the laser
			glDisable(GL_LIGHTING);

				laser.draw();

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
};

const GLfloat BlocksView::groundPlaneSize = 200.0;
const double BlocksView::PI = 3.141592654;
const double BlocksView::angle = PI / 4.0;
const double BlocksView::angle2 = 5.0 *  PI / 4.0;
const float BlocksView::radius = 300.0;

const GLfloat BlocksView::light0Position[4] = {radius * cos(angle), 600.0, radius * sin(angle), 1.0f};
const GLfloat BlocksView::light0SpecularIntensity[4] = {1.0f, 1.0f, 1.0f, 1.0f};
const GLfloat BlocksView::light0DiffuseIntensity[4] = {1.0, 1.0, 1.0, 1.0};
const GLfloat BlocksView::light0AmbientIntensity[4] = {0.2f, 0.2f, 0.2f, 1.0f};

//Spot Light
const GLfloat BlocksView::light1Position[4] = {radius * cos(angle2), 600.0, radius * sin(angle2), 1.0f};
const GLfloat BlocksView::light1Direction[4] = {0.0, -1.0, 0.0, 0.0f};
const GLfloat BlocksView::light1SpecularIntensity[4] = {1.0f, 1.0f, 1.0f, 1.0f};
const GLfloat BlocksView::light1DiffuseIntensity[4] = {1.0, 1.0, 1.0, 1.0};
const GLfloat BlocksView::light1AmbientIntensity[4] = {0.2f, 0.2f, 0.2f, 1.0f};

//Dim Point Light (at same position as first point light)
const GLfloat BlocksView::light2Position[4] = {radius * cos(angle), 600.0, radius * sin(angle), 1.0f};
const GLfloat BlocksView::light2SpecularIntensity[4] = {0.0f, 0.0f, 0.0f, 1.0f};
const GLfloat BlocksView::light2DiffuseIntensity[4] = {0.2, 0.2, 0.2, 1.0};
const GLfloat BlocksView::light2AmbientIntensity[4] = {0.0f, 0.0f, 0.0f, 1.0f};

const GLfloat BlocksView::Material1Specular[4] = {1.0, 1.0, 1.0, 1.0};
const GLfloat BlocksView::Material1Diffuse[4] = {0.2, 0.2, 0.2, 1.0};
const GLfloat BlocksView::Material1Ambient[4] = {0.2, 0.2, 0.2, 1.0};
const GLfloat BlocksView::Material1Shininess = 70.0;

const GLfloat BlocksView::Material2Specular[4] = {0.2, 0.2, 0.2, 1.0};
const GLfloat BlocksView::Material2Diffuse[4] = {1.0, 1.0, 1.0, 1.0};
const GLfloat BlocksView::Material2Ambient[4] = {0.2, 0.2, 0.2, 1.0};
const GLfloat BlocksView::Material2Shininess = 10.0;

#endif /*BLOCKSVIEW_H_*/
