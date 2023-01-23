#ifndef POROUSBLOCKACTIVE_H_
#define POROUSBLOCKACTIVE_H_

#include "AbstractBlockState.h"

/**
  * @brief Defines the active state for PorousBlock.  When this state is set, the block changes color.
  */
class PorousBlockActive : public AbstractBlockState
{
	public:
		PorousBlockActive(AbstractBlock& block) : AbstractBlockState(block)
		{
			block.setColor(Vector4(1.0, 0.5, 0.0, 1.0));
		}
};

#endif /*POROUSBLOCKACTIVE_H_*/
