#ifndef SCROLLUPCOMMAND_H_
#define SCROLLUPCOMMAND_H_

#include "AbstractCommand.h"
#include "ScrollingButtonMenu.h"

class ScrollingButtonMenu;

/**
  * @brief This Command controls a ScrollingButtonMenu by scrolling it upward.
  */
class ScrollUpCommand : public AbstractCommand
{
	private:
		ScrollingButtonMenu& menu;
		
	public:
		ScrollUpCommand(ScrollingButtonMenu& menu) : menu(menu) {}
		
		virtual void execute() const
		{
			menu.scrollUp();
		}
};

#endif /*SCROLLUPCOMMAND_H_*/
