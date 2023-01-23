#ifndef BLOCKEXPLODE_H_
#define BLOCKEXPLODE_H_

#include <cstdlib> 

#include "AbstractBlockState.h"

/**
  * @brief When this state is set, the block is powerfully forced in a random direction.
  */
class BlockExplode : public AbstractBlockState
{
	private:

	public:
		BlockExplode(AbstractBlock& block) : AbstractBlockState(block)
		{
		}
};

#endif /*BLOCKEXPLODE_H_*/
