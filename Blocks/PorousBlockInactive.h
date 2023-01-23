#ifndef POROUSBLOCKINACTIVE_H_
#define POROUSBLOCKINACTIVE_H_

#include "AbstractBlockState.h"

/**
  * @brief This class defines the inactive state for PorousBlock.  In this state, the collision hull is kinematic.
  */
class PorousBlockInactive : public AbstractBlockState
{
	public:
		PorousBlockInactive(AbstractBlock& block) : AbstractBlockState(block)
		{
			block.setColor(Vector4(0.5, 0.0, 1.0, 1.0));
			block.setKinematic(true);
		}
};

#endif /*POROUSBLOCKINACTIVE_H_*/
