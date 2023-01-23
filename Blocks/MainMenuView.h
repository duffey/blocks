#ifndef MAINMENUVIEW_H_
#define MAINMENUVIEW_H_

#include "AbstractView.h"
#include "ScrollingButtonMenu.h"

/**
  * @brief This class is a part of the implementation of the %Model View %Controller pattern
  * This is the basic View for selecting a game to play.
  */
class MainMenuView : public AbstractView
{
	private:
		const ScrollingButtonMenu& menu;

	public:
		MainMenuView(const GLfloat& windowWidth, const GLfloat& windowHeight, const ScrollingButtonMenu& menu) : AbstractView(windowWidth, windowHeight), menu(menu) {}

		virtual void display() const
		{
			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glEnable(GL_DEPTH_BUFFER_BIT);
			glDisable(GL_LIGHTING);

			glMatrixMode (GL_MODELVIEW);
			glPushMatrix();
			glLoadIdentity ();
				
			glMatrixMode(GL_PROJECTION);
			glPushMatrix();

				glLoadIdentity();
				gluOrtho2D(0, getWindowWidth(), 0, getWindowHeight());
				glDisable(GL_LIGHTING);
				glDisable(GL_DEPTH_TEST);
				glDepthFunc(GL_ALWAYS);
				glEnable(GL_BLEND);
				
					glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
					
					menu.draw();
				
				glDisable(GL_BLEND);
				glDepthFunc(GL_LEQUAL);
				glEnable(GL_DEPTH_TEST);
				glEnable(GL_LIGHTING);
				
			glPopMatrix();
				
			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();
		}
};

#endif /*MAINMENUVIEW_H_*/
