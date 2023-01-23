#include <algorithm>	//min

#include "ScrollingButtonMenu.h"
#include "NullCommand.h"
#include "ScrollUpCommand.h"
#include "ScrollDownCommand.h"

ScrollingButtonMenu::ScrollingButtonMenu(GLfloat x, GLfloat y, GLfloat buttonWidth, GLfloat buttonHeight,
					size_type visibleButtons = defaultVisibleButtons,
					GLfloat padding = 0.0) :

					x(x),
					y(y),
					buttonWidth(buttonWidth),
					buttonHeight(buttonHeight),
					visibleButtons(visibleButtons),
					padding(padding),
					scrollUpButton(x + padding, y - padding, buttonWidth, buttonHeight, new ScrollUpCommand(*this), "^"),
					scrollDownButton(x + padding, y - padding * (visibleButtons + 2) - buttonHeight * (visibleButtons + 1), buttonWidth, buttonHeight, new ScrollDownCommand(*this), "v"),
					visibleStart(0) {}

vector<Button>::iterator ScrollingButtonMenu::begin() { return buttons.begin(); }

vector<Button>::const_iterator ScrollingButtonMenu::begin() const { return buttons.begin(); }

vector<Button>::iterator ScrollingButtonMenu::end() { return buttons.end(); }

vector<Button>::const_iterator ScrollingButtonMenu::end() const { return buttons.end(); }

void ScrollingButtonMenu::scrollUp()
{
	GLfloat buttonPositionX = x + padding;
	GLfloat buttonPositionY = y - 2.0 * padding - buttonHeight; //Start below the scroll up button

	if(buttons.size() > visibleButtons && visibleStart != 0)
	{
		--visibleStart;

		for(size_type i = visibleStart; i != visibleStart + min(buttons.size(), visibleButtons); i++)
		{
			buttons.at(i).setX(buttonPositionX);
			buttons.at(i).setY(buttonPositionY);

			buttonPositionY -= buttonHeight + padding;
		}
	}
}

void ScrollingButtonMenu::scrollDown()
{
	GLfloat buttonPositionX = x + padding;
	//Start below the scroll up button
	GLfloat buttonPositionY = y - 2.0 * padding - buttonHeight;

	if(buttons.size() > visibleButtons && visibleStart + visibleButtons != buttons.size())
	{
		++visibleStart;

		for(size_type i = visibleStart; i != visibleStart + min(buttons.size(), visibleButtons); i++)
		{
			buttons.at(i).setX(buttonPositionX);
			buttons.at(i).setY(buttonPositionY);

			buttonPositionY -= buttonHeight + padding;
		}
	}
}

void ScrollingButtonMenu::sendMouseUp(GLfloat mouseX, GLfloat mouseY)
{
	if(visibleStart != 0)
		scrollUpButton.sendMouseUp(mouseX, mouseY);
	
	if(visibleStart + visibleButtons != buttons.size())
		scrollDownButton.sendMouseUp(mouseX, mouseY);
	
	for(size_type i = visibleStart; i != visibleStart + min(buttons.size(), visibleButtons); i++)
		buttons.at(i).sendMouseUp(mouseX, mouseY);
}

void ScrollingButtonMenu::sendMouseDown(GLfloat mouseX, GLfloat mouseY)
{
	if(visibleStart != 0)
		scrollUpButton.sendMouseDown(mouseX, mouseY);
	
	if(visibleStart + visibleButtons != buttons.size())
		scrollDownButton.sendMouseDown(mouseX, mouseY);
	
	for(size_type i = visibleStart; i != visibleStart + min(buttons.size(), visibleButtons); i++)
		buttons.at(i).sendMouseDown(mouseX, mouseY);
}

void ScrollingButtonMenu::add(const Button& button)
{
	Button temp = button;

	temp.setWidth(buttonWidth);
	temp.setHeight(buttonHeight);

	if(buttons.size() < visibleButtons)
	{
		temp.setX(x + padding);
		//We must account for the scroll up button
		temp.setY(y - padding * (buttons.size() + 2) - buttonHeight * (buttons.size() + 1));
	}

	buttons.push_back(temp);
}

void ScrollingButtonMenu::draw() const
{
	if(buttons.size() > visibleButtons)
	{
		if(visibleStart != 0)
			scrollUpButton.draw();

		if(visibleStart + visibleButtons != buttons.size())
			scrollDownButton.draw();
	}

	for(size_type i = visibleStart; i != visibleStart + min(buttons.size(), visibleButtons); i++)
		buttons.at(i).draw();
}