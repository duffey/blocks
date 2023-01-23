#ifndef BLOCKLIMP_H_
#define BLOCKLIMP_H_

#include "AbstractBlockState.h"

/**
  * @brief When this state is set, the block is weakly forced in a random downward direction,
  * giving something such as a structure of blocks a crumbling effect.
  */
class BlockLimp : public AbstractBlockState
{
	private:
		static const NxReal forceStrength;

	public:
		BlockLimp(AbstractBlock& block) : AbstractBlockState(block)
		{
		}
};

const NxReal BlockLimp::forceStrength = 20.0;

#endif /*BLOCKKINEMATIC_H_*/
