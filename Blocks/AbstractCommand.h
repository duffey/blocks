#ifndef ABSTRACTCOMMAND_H_
#define ABSTRACTCOMMAND_H_

/**
  * @brief This class models the Command Design Pattern
  */
class AbstractCommand
{
	public:
		virtual ~AbstractCommand() {}

		/**
		  * This method is implemented by all commands.
		  */
		virtual void execute() const = 0;
};

#endif /*ABSTRACTCOMMAND_H_*/
