#ifndef IMPERMEABLEBLOCKINACTIVE_H_
#define IMPERMEABLEBLOCKINACTIVE_H_

#include "AbstractBlockState.h"

/**
  * @brief This class defines ImpermeableBlock's inactive state.  In this state, the block is simulated in kinematic mode.
  */
class ImpermeableBlockInactive : public AbstractBlockState
{
	public:
		ImpermeableBlockInactive(AbstractBlock& block) : AbstractBlockState(block)
		{
			block.setColor(Vector4(1.0, 1.0, 1.0, 1.0));
		}
};

#endif /*IMPERMEABLEBLOCKINACTIVE_H_*/
