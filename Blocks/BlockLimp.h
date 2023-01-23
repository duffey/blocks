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
			block.setKinematic(false);
			block.applyForce(NxVec3((GLfloat)rand() / RAND_MAX * 2.0 - 1.0, (GLfloat)rand() / RAND_MAX * -1.0, (GLfloat)rand() / RAND_MAX * 2.0 - 1.0), forceStrength);
		}
};

const NxReal BlockLimp::forceStrength = 20.0;

#endif /*BLOCKKINEMATIC_H_*/
