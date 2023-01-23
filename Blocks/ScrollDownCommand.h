#ifndef SCROLLDOWNCOMMAND_H_
#define SCROLLDOWNCOMMAND_H_

#include "AbstractCommand.h"
#include "ScrollingButtonMenu.h"

/**
  * @brief This Command controls a ScrollingButtonMenu by scrolling it downward.
  */
class ScrollDownCommand : public AbstractCommand
{
	private:
		ScrollingButtonMenu& menu;
		
	public:
		ScrollDownCommand(ScrollingButtonMenu& menu) : menu(menu) {}
		
		virtual void execute() const
		{
			menu.scrollDown();
		}
};

#endif /*SCROLLDOWNCOMMAND_H_*/
