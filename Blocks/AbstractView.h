#ifndef ABSTRACTVIEW_H_
#define ABSTRACTVIEW_H_

/**
  * @brief This class is a part of the implementation of the %Model View %Controller pattern
  */
class AbstractView
{
	private:
		GLfloat windowWidth, windowHeight;

	public:
		AbstractView(const GLfloat& windowWidth, const GLfloat& windowHeight) : windowWidth(windowWidth), windowHeight(windowHeight) {}

		virtual ~AbstractView() {}

		virtual void display() const = 0;

		void setWindowWidth(const GLfloat& windowWidth) { this -> windowWidth = windowWidth; }

		void setWindowHeight(const GLfloat& windowHeight) { this -> windowHeight = windowHeight; }

		const GLfloat& getWindowWidth() const { return windowWidth; }

		const GLfloat& getWindowHeight() const { return windowHeight; }
};

#endif /*ABSTRACTVIEW_H_*/
