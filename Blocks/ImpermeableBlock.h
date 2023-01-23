#ifndef IMPERMEABLEBLOCK_H_
#define IMPERMEABLEBLOCK_H_

#include "AbstractBlock.h"
#include "ImpermeableBlockInactive.h"
#include "BlockLimp.h"
#include "BlockExplode.h"

/**
  * @brief This class defines a plain-vanilla block that cannont be penetrated.
  */
class ImpermeableBlock : public AbstractBlock
{
	public:
		ImpermeableBlock(GLfloat size = 1.0, const Matrix44& globalOrientation = Matrix44()) : AbstractBlock(size, globalOrientation)
		{
			setState(new ImpermeableBlockInactive(*this));
		}

		virtual void setVictoryState() { setState(new BlockExplode(*this)); }

		virtual void setLossState() { setState(new BlockLimp(*this)); }
		
		virtual ImpermeableBlock* clone () const { return new ImpermeableBlock(*this); }
		
		virtual bool isImpenetrable() const { return true; }
       	
	private:
		virtual void touchImplementation() {}
};

#endif /*IMPERMEABLEBLOCK_H_*/
