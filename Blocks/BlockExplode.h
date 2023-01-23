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
			block.setKinematic(false);
			block.applyForce(NxVec3((GLfloat)rand() / RAND_MAX * 2.0 - 1.0, (GLfloat)rand() / RAND_MAX * 2.0 - 1.0, (GLfloat)rand() / RAND_MAX * 2.0 - 1.0), forceStrength);
		}
};

const NxReal BlockExplode::forceStrength = 400.0;

#endif /*BLOCKEXPLODE_H_*/
