#ifndef ABSTRACTBLOCK_H_
#define ABSTRACTBLOCK_H_

#include <iostream>	//NOTE: REMOVE

#include "Vector4.h"
#include "Matrix44.h"
#include "AbstractBlockState.h"
#include "Cube.h"
#include "NxPhysics.h"

/**
  * @brief This class describes the interface for all blocks in the game.
  */
class AbstractBlock
{
	private:
		bool touched;
		NxScene& physxScene;
		const GLfloat size;
		Matrix44 globalOrientation; //NOTE: REMOVE
		AbstractBlockState* state;
		Cube model;

	public:
		AbstractBlock(NxScene& physxScene, GLfloat size = 1.0, const Matrix44& globalOrientation = Matrix44()) : physxScene(physxScene), touched(false), size(size), globalOrientation(globalOrientation), state(NULL), model(physxScene, this -> size, globalOrientation)
		{
			setKinematic(true);
		}

		virtual ~AbstractBlock()
		{
			if(state != NULL)
				delete state;
		}
		
		/**
		  * Sets the state of the block to "touched"
		  */
		void touch()
		{
			touched = true;
			
			touchImplementation();
		}

		void setKinematic(bool kinematic) { model.setKinematic(kinematic); }

		void applyForce(const NxVec3& forceDirection, const NxReal forceStrength) { model.applyForce(forceDirection, forceStrength); }

		virtual void setVictoryState() = 0;

		virtual void setLossState() = 0;
		
		/**
		  * @return true if the block has been touched
		  */
		bool hasBeenTouched() const { return touched; }
		
		/**
		  * @return true if the block is an obstacle
		  */
		virtual bool isImpenetrable() const = 0;
		
		/**
		  * @see Handle
		  * @return a pointer to a duplicate object for use with Handle
		  */
		virtual AbstractBlock* clone () const = 0;
		
		/**
		  * Draws the block
		  */
		void draw() const { model.draw(); }
		
		/**
		  * Draws a "Shadow Volume" for use with the stenciled shadow volume algorithm
		  * @param lightPosition the light source which determines how edges are extruded to form the shadow volume
		  */
		void drawShadowVolume(const Vector4& lightPosition) const { model.drawShadowVolume(lightPosition); }
		
		/**
		  * @return A Vector4 containing the x, y, and z coordinates of the block with respect to the standard basis
		  */
		/*const Vector4 getLocation() const
		{
			//NOTE: Need to do a change of basis to the standard basis
			return Vector4();
		}*/
		
		/**
		  * @return the block's Modelview transformation
		  */
		const Matrix44& getOrientation() const { return model.getOrientation(); }
		
		/**
		  * @return the size of the block
		  */
		const GLfloat& getSize() const { return size; }
		
		/**
		  * Sets the block's color for when it is drawn
		  * @see draw
		  * @param color a vector containing the red, green, blue, and alpha components of the color
		  */
		void setColor(const Vector4& color) { model.setColor(color); }
		
		/*virtual bool equals (const AbstractBlock& other) const
       	{
       		std::cout << "AbstractBlock::equals" << endl;
    		return size == other.size;
    	}
		
		friend bool operator == (const AbstractBlock& lhs, const AbstractBlock& rhs)
		{
        	return lhs.equals(rhs);
       	}
       	
       	friend bool operator != (const AbstractBlock& lhs, const AbstractBlock& rhs)
		{
        	return !(lhs == rhs);
       	}*/
       	
	private:
		/**
		  * Defines what happens when a block is touched
		  */
		virtual void touchImplementation() = 0;
		
	protected:
		/**
		  * Sets the state of the block, which may affect gameplay elements as well as how it is drawn on the screen
		  * @param state An instance of a child of AbstractBlockState, which defines the state behavior
		  */
		void setState(AbstractBlockState* state)
		{
			if(this -> state != NULL && this -> state != state)
				delete this -> state;
				
			this -> state = state;
		}
		
		/**
		  * @return an object which describes the block's internal state
		  */
		const AbstractBlockState* const getState() const { return state; }
};

#endif /*ABSTRACTBLOCK_H_*/
