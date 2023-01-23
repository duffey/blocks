#ifndef ABSTRACTBLOCKSTATE_H_
#define ABSTRACTBLOCKSTATE_H_

#include "AbstractBlock.h"
#include "Matrix44.h"

class AbstractBlock;

/**
  * @brief This class assists in the implementation of the State Pattern for AbstractBlock.
  */
class AbstractBlockState
{
	private:
		AbstractBlock& block;
		
	public:
		AbstractBlockState(AbstractBlock& block) : block(block) {}

		virtual ~AbstractBlockState() {}
		
	protected:
		/**
		  * This method allows children of AbstractBlockState to obtain a reference to the block they describe in order to modify its behavior.
		  * @return a reference to the block described by this state
		  */
		const AbstractBlock& getBlock() const { return block; }
};

#endif /*ABSTRACTBLOCKSTATE_H_*/
