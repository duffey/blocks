#ifndef NULLCOMMAND_H_
#define NULLCOMMAND_H_

#include "AbstractCommand.h"

/**
  * @brief This Command does nothing and can be used as a default command.
  */
class NullCommand : public AbstractCommand
{
	public:
		virtual void execute() const {}
};

#endif /*NULLCOMMAND_H_*/
