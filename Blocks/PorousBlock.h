#ifndef POROUSBLOCK_H_
#define POROUSBLOCK_H_

#include <cassert>	//assert

#include "AbstractBlock.h"
#include "PorousBlockInactive.h"
#include "PorousBlockActive.h"
/**
  * @brief This class defines blocks which can be penetrated.  Additionally, these blocks change color when they are touched.
  */
class PorousBlock : public AbstractBlock
{
	private:
		Vector4 color1, color2;

	public:
		PorousBlock(GLfloat size = 1.0, const Matrix44& globalOrientation = Matrix44(), Vector4 color1 = Vector4(0.5, 0.0, 1.0, 1.0), Vector4 color2 = Vector4(0.0, 1.0, 0.0, 1.0)) : AbstractBlock(size, globalOrientation), color1(color1), color2(color2)
		{
			setState(new PorousBlockInactive(*this));
		}

		virtual void setVictoryState() {  }

		virtual void setLossState() {  }
		
		virtual PorousBlock* clone () const { return new PorousBlock(*this); }
		
		/*virtual bool equals (const AbstractBlock& other)
		{
			return AbstractBlock::equals(other);
		}*/
		
		virtual bool isImpenetrable() const { return false; }
		
		const Vector4& getColor1() const { return color1; }
		
		const Vector4& getColor2() const { return color2; }

	private:
		virtual void touchImplementation()
		{
			setState(new PorousBlockActive(*this));
		}
};

#endif /*POROUSBLOCK_H_*/
