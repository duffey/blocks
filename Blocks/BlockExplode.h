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
		static const NxReal forceStrength;

	public:
		BlockExplode(AbstractBlock& block) : AbstractBlockState(block)
		{
		}
};

const NxReal BlockExplode::forceStrength = 400.0;

#endif /*BLOCKEXPLODE_H_*/
