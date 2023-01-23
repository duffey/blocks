#ifndef ABSTRACTBLOCK_H_
#define ABSTRACTBLOCK_H_

#include <iostream>	//NOTE: REMOVE

#include "Vector4.h"
#include "Matrix44.h"
#include "Cube.h"

/**
  * @brief This class describes the interface for all blocks in the game.
  */
class Block
{
	private:
		bool touched;
		const GLfloat size;
		Matrix44 globalOrientation; //NOTE: REMOVE

		Cube model;
		bool isPenatrable;

	public:
		Block(bool isPenatrable, GLfloat size = 1.0, const Matrix44& globalOrientation = Matrix44()) : isPenatrable(isPenatrable), touched(false), size(size), globalOrientation(globalOrientation), model(this -> size, globalOrientation)
		{
			if (isPenatrable) {
				setColor(Vector4(0.5, 0.0, 1.0, 1.0));
			}
			else {
				setColor(Vector4(1.0, 1.0, 1.0, 1.0));
			}
		}
		
		/**
		  * Sets the state of the block to "touched"
		  */
		void touch()
		{
			touched = true;
			
			if (isPenatrable) {
				setColor(Vector4(1.0, 0.5, 0.0, 1.0));
			}
		}

		
		/**
		  * @return true if the block has been touched
		  */
		bool hasBeenTouched() const { return touched; }
		
		/**
		  * @return true if the block is an obstacle
		  */
		bool isImpenetrable() {
			return !isPenatrable;
		}
		
		
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
       	
};

#endif /*ABSTRACTBLOCK_H_*/
