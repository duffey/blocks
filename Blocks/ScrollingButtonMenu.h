#ifndef SCROLLINGBUTTONMENU_H_
#define SCROLLINGBUTTONMENU_H_

#include <vector>
#include <string>		//string
#include <algorithm>	//max

#include "Button.h"

using namespace std;

/*	.....................................
	.ppppppppppppppppppppppppppppppppppp.
	.p                                 p.
	.p                ^                p.
	.p                                 p.
	.ppppppppppppppppppppppppppppppppppp.
	.p                                 p.
	.p             button              p.
	.p                                 p.
	.ppppppppppppppppppppppppppppppppppp.
	.p                                 p.
	.p             button              p.
	.p                                 p.
	.ppppppppppppppppppppppppppppppppppp.
	.p                                 p.
	.p             button              p.
	.p                                 p.
	.ppppppppppppppppppppppppppppppppppp.
	.p                                 p.
	.p                v                p.
	.p                                 p.
	.ppppppppppppppppppppppppppppppppppp.
	.....................................*/

/**
  * @brief This class defines a menu of Button objects which can be scrolled through mouse input.
  */
class ScrollingButtonMenu
{
	public:
		typedef vector<Button>::size_type size_type;
		typedef vector<Button>::iterator iterator;
	    typedef vector<Button>::const_iterator const_iterator;
		string path;
		bool pathset = false;

	private:
		static const size_type defaultVisibleButtons = 5;

		GLfloat x, y, buttonWidth, buttonHeight;
		size_type visibleButtons;
		GLfloat padding;

		Button scrollUpButton, scrollDownButton;
		vector<Button> buttons;
		size_type visibleStart;

	public:
		ScrollingButtonMenu(GLfloat x, GLfloat y, GLfloat buttonWidth, GLfloat buttonHeight,
							size_type visibleButtons,
							GLfloat padding);

		iterator begin();

		const_iterator begin() const;

		iterator end();

		const_iterator end() const;

		/*
		 * This method repositions all of the buttons in view with respect to OpenGL's screen space coordinate system,
		 * whereby the coordinate (0, 0) is located in the bottom left corner of the screen.
		 */
		void scrollUp();

		/*
		 * This method repositions all of the buttons in view with respect to OpenGL's screen space coordinate system,
		 * whereby the coordinate (0, 0) is located in the bottom left corner of the screen.
		 */
		void scrollDown();
		
		void sendMouseUp(GLfloat mouseX, GLfloat mouseY);
		
		void sendMouseDown(GLfloat mouseX, GLfloat mouseY);

		void add(const Button& button);

		void draw() const;
};

#endif /*SCROLLINGBUTTONMENU_H_*/
