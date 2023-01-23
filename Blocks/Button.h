#ifndef BUTTON_H_
#define BUTTON_H_

#if __APPLE__
#include <GLUT/glut.h>
#else 
#include <GL/glut.h>
#endif

#include <string>	//string

#include "AbstractCommand.h"
#include "NullCommand.h"
#include "Vector4.h"

using namespace std;

/**
  * @brief This class creates instances of buttons, which contain a Command (Command Pattern) to be executed.
  */
class Button
{	
	public:
		//NOTE: Could benefit from the State Pattern
		enum State { UP, MOUSE_OVER, DOWN };

	private:
		//The x and y coordinates of the top left corner of the button's hit box and the hit box's width and height
		GLfloat x, y, width, height;
		AbstractCommand* command;
		string text;
		State state;

	public:
		Button(AbstractCommand* command, string text = "") : x(0.0), y(0.0), width(0.0), height(0.0), command(command), text(text), state(UP) {}

		Button(GLfloat x, GLfloat y, GLfloat width, GLfloat height, AbstractCommand* command = new NullCommand(), string text = "") : x(x), y(y), width(width), height(height), command(command), text(text), state(UP) {}

		Button(const Button& other) : x(other.x), y(other.y), width(other.width), height(other.height), command(other.command), text(other.text), state(other.state) {}
		
		//API specific
		//Must give OpenGL viewport mouse coordinates
		bool isTouching(GLfloat mouseX, GLfloat mouseY) const
		{
			return x <= mouseX && mouseX <= x + width && y >= mouseY && mouseY >= y - height;
		}

		void setX(GLfloat x) { this -> x = x; }

		void setY(GLfloat y) { this -> y = y; }

		void setWidth(GLfloat width) { this -> width = width; }

		void setHeight(GLfloat height) { this -> height = height; }

		void setText(string text) { this -> text = text; }

		void setState(State state) { this -> state = state; }
		
		const GLfloat& getX() const { return x; }

		const GLfloat& getY() const { return y; }

		const GLfloat& getWidth() const { return width; }

		const GLfloat& getHeight() const { return height; }

		const string& getText() const { return text; }
		
		const State& getState() const { return state; }
		
		//API specific
		void sendMouseUp(GLfloat mouseX, GLfloat mouseY)
		{
			if(isTouching(mouseX, mouseY))
			{
				if(getState() == DOWN)
					command -> execute();
				
				setState(MOUSE_OVER);
			}
			else
				setState(UP);
		}
		
		//API specific
		void sendMouseDown(GLfloat mouseX, GLfloat mouseY)
		{
			if(getState() == DOWN || (isTouching(mouseX, mouseY) && getState() == MOUSE_OVER))
				setState(DOWN);
			else
				setState(UP);
		}

		/*
		 * This method assumes that a 2D orthographic projection has already been set up for drawing elements on the screen.
		 */
		void draw() const
		{
			static Vector4 color1(0.0, 0.0, 0.0, 1.0);
			static Vector4 color2(1.0, 1.0, 1.0, 1.0);

			switch(state)
			{
				case UP:			drawButtonHelper(color1, color2);
									
									break;
							
				case MOUSE_OVER:	drawButtonHelper(color2, color1);
									
									break;
									
				case DOWN:			drawButtonHelper(color2, color1);

									break;
			}
		}

	private:
		void drawButtonHelper(const Vector4& buttonColor, const Vector4& textColor) const
		{
			glColor4fv(buttonColor.data());
			glBegin(GL_QUADS);

				glVertex2f(x, y);
				glVertex2f(x, y - height);
				glVertex2f(x + width, y - height);
				glVertex2f(x + width, y);

			glEnd();

			glPushMatrix();
			
				glTranslatef(x + 5.0, y - 25.0 - 5.0, 0.0);
				glScalef(0.25, 0.25, 0.25);
				glColor4fv(textColor.data());
				glLineWidth(1.0);

				for(string::const_iterator i = text.begin(); i != text.end(); i++)
					glutStrokeCharacter(GLUT_STROKE_ROMAN, *i);

			glPopMatrix();
		}
};

#endif /*BUTTON_H_*/
