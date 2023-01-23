#ifndef BLOCKSTRUCTURE_H_
#define BLOCKSTRUCTURE_H_

#include <cstddef>		//size_t
#include <cassert>		//assert
#include <fstream>		//ifstream
#include <string>		//string
#include <stdexcept>	//runtime_error
#include <iostream>		//istream

#include "AbstractBlock.h"
#include "PorousBlock.h"
#include "ImpermeableBlock.h"
#include "Vector4.h"
#include "Matrix44.h"
#include "Handle.h"

typedef Handle<AbstractBlock> Block;

using namespace std;

/**
  * @brief This class models a structure of block objects.
  * @see AbstractBlock
  */
class BlockStructure
{
	public:
		typedef size_t size_type;

	private:
		enum { x, y, z, w };
		size_type height, rows, columns;
		GLfloat blockSize;
		Vector4 base;
		Block*** blocks;

	public:
		BlockStructure(const string filePath, GLfloat blockSize = 1.0, const Vector4& base = Vector4(0.0, 0.0, 0.0, 1.0)) : height(0.0), rows(0.0), columns(0.0), blockSize(blockSize), base(base[x], base[y], base[z], 0.0), blocks(0)
		{
			assert(base[w] == 1.0);

			filePath >> *this;
		}

		~BlockStructure()
		{
			for(size_type i = 0; i < height; i++)
			{
				for(size_type j = 0; j < rows; j++)
					delete[] blocks[i][j];

				delete[] blocks[i];
			}

			delete[] blocks;
		}

		/**
		  * Draws the BlockStructure
		  */
		void draw() const
		{
			for(size_type i = 0; i < height; i++)
				for(size_type j = 0; j < rows; j++)
					for(size_type k = 0; k < columns; k++)
						if(hasBlock(i, j, k))
							blocks[i][j][k] -> draw();
		}
		
		/**
		  * Draws a "Shadow Volume" for use with the stenciled shadow volume algorithm
		  * @param lightPosition the light source which determines how edges are extruded to form the shadow volume
		  */
		void drawShadowVolume(const Vector4& lightPosition) const
		{
			for(size_type i = 0; i < height; i++)
				for(size_type j = 0; j < rows; j++)
					for(size_type k = 0; k < columns; k++)
						if(hasBlock(i, j, k))
							blocks[i][j][k] -> drawShadowVolume(lightPosition);
		}
		
		/**
		  * Sets the state of a particular block to "touched."
		  * This method throws a runtime_error if a block does not exist at the specified location.
		  * @see hasBlock
		  * @param height the height of the block in the grid
		  * @param row the row of the block in the grid
		  * @param column the column of the block in the grid
		  */
		void touchBlock(size_type height, size_type row, size_type column)
		{
			if(!hasBlock(height, row, column))
				throw runtime_error("A block does not exist at the specified location.");

			assert(hasBlock(height, row, column));

			blocks[height][row][column] -> touch();
		}

		void setBlockLossState(size_type height, size_type row, size_type column)
		{
			if(!hasBlock(height, row, column))
				throw runtime_error("A block does not exist at the specified location.");

			assert(hasBlock(height, row, column));

			blocks[height][row][column] -> setLossState();
		}

		void setBlockVictoryState(size_type height, size_type row, size_type column)
		{
			if(!hasBlock(height, row, column))
				throw runtime_error("A block does not exist at the specified location.");

			assert(hasBlock(height, row, column));

			blocks[height][row][column] -> setVictoryState();
		}
		
		/**
		  * @return true if a child of AbstractBlock exists at the specified location
		  * @see AbstractBlock
		  * @param height the height in the grid
		  * @param row the row in the grid
		  * @param column the column in the grid
		  */
		bool hasBlock(size_type height, size_type row, size_type column) const
		{
			return isInBounds(height, row, column) && blocks[height][row][column] != NULL;
		}
		
		//NOTE: Why don't we make a const Block& getBlock(size_type, size_type, size_type) method?
		bool hasImpenetrableBlock(size_type height, size_type row, size_type column) const
		{
			return isInBounds(height, row, column) && blocks[height][row][column] != NULL && blocks[height][row][column] -> isImpenetrable();
		}

		bool hasTouchedBlock(size_type height, size_type row, size_type column)
		{
			return hasBlock(height, row, column) && blocks[height][row][column] -> hasBeenTouched();
		}
		
		const AbstractBlock& getBlock(size_type height, size_type row, size_type column) const
		{
			assert(hasBlock(height, row, column));
			
			return *blocks[height][row][column];
		}
		
		/**
		  * returns a vector containing the x, y, and z coordinates of the block with respect to the standard basis
		  */
		const Vector4 getBlockLocation(size_type height, size_type row, size_type column) const
		{
			if(!hasBlock(height, row, column))
				throw runtime_error("A block does not exist at the specified location.");
			
			//NOTE: WE SHOULD BE ABLE TO REPLACE THIS WITH A CALL TO A GETLOCATION METHOD THAT RESIDES WITHIN BLOCK 
			//NOTE: Need to stop computing this so many times
			GLfloat upperLeftCornerX = base[x] - (columns / 2) * blockSize, upperLeftCornerZ = base[z] - (rows / 2) * blockSize;

			if(columns % 2 == 0)
				upperLeftCornerX += blockSize / 2.0;

			if(rows % 2 == 0)
				upperLeftCornerZ += blockSize / 2.0;
			
			return Vector4(	upperLeftCornerX + column * blockSize,
							base[y] + blockSize / 2.0 + blockSize * height,
							upperLeftCornerZ + row * blockSize,
							1.0);
		}
		
		/**
		  * @return the size of the blocks that exist within the structure
		  */
		const GLfloat& getBlockSize() const { return blockSize; }
		
		/**
		  * @return the number of levels occupied by blocks within the structure
		  */
		const size_type& getHeight() const { return height; }
		
		/**
		  * @return the number of rows occupied by blocks within the structure
		  */
		const size_type& getRows() const { return rows; }
		
		/**
		  * @return the number of columns occupied by blocks within the structure
		  */
		const size_type& getColumns() const { return columns; }
		
		/**
		  * @return A positional vector describing the location at the center of the BlockStructure's base
		  */
		const Vector4& getBase() const { return base; }

		friend void operator >> (const string filePath, BlockStructure& blockStructure)
		{
			ifstream in(filePath.c_str());
			
			if(in.fail())
				throw runtime_error("The specified file '" + string(filePath) + "' does not exist.");

			cout << "file exists " << filePath << endl;
	

			//Deallocate any existing blocks
			for(size_type i = 0; i < blockStructure.height; i++)
			{
				for(size_type j = 0; j < blockStructure.rows; j++)
					delete[] blockStructure.blocks[i][j];

				delete[] blockStructure.blocks[i];
			}

			delete[] blockStructure.blocks;

			Matrix44 blockOrientation;
			GLfloat upperLeftCornerX, upperLeftCornerZ;

			in >> blockStructure.height;
			in >> blockStructure.rows;
			in >> blockStructure.columns;

			cout << "height " << blockStructure.height << endl;

			blockStructure.blocks = new Block**[blockStructure.height];
			
			upperLeftCornerX = (blockStructure.columns / 2) * -blockStructure.blockSize;
			upperLeftCornerZ = (blockStructure.rows / 2) * -blockStructure.blockSize;

			if(blockStructure.columns % 2 == 0)
				upperLeftCornerX += blockStructure.blockSize / 2.0;

			if(blockStructure.rows % 2 == 0)
				upperLeftCornerZ += blockStructure.blockSize / 2.0;
			
			glMatrixMode(GL_MODELVIEW);
			
			glPushMatrix();
				glLoadIdentity();
				glTranslatef(blockStructure.base[x], blockStructure.base[y], blockStructure.base[z]);
				
				for(size_type i = 0; i < blockStructure.height; i++)
				{
					blockStructure.blocks[i] = new Block*[blockStructure.rows];
	
					for(size_type j = 0; j < blockStructure.rows; j++)
					{	
						blockStructure.blocks[i][j] = new Block[blockStructure.columns];
	
						for(size_type k = 0; k < blockStructure.columns; k++)
						{
							glPushMatrix();
								glTranslatef(upperLeftCornerX + k * blockStructure.blockSize, blockStructure.blockSize / 2.0 + blockStructure.blockSize * i, upperLeftCornerZ + j * blockStructure.blockSize);
								glGetFloatv(GL_MODELVIEW_MATRIX, blockOrientation);
							glPopMatrix();
	
							char blockType;
	
							in >> blockType;
	
							switch(blockType)
							{
							case 'P':	
											blockStructure.blocks[i][j][k] = new PorousBlock(blockStructure.blockSize, blockOrientation);	
											break;
											
								case 'I' :	blockStructure.blocks[i][j][k] = new ImpermeableBlock(blockStructure.blockSize, blockOrientation);	
											break;
								
								default :	blockStructure.blocks[i][j][k] = NULL;
							}
						}
					}
				}
				
			glPopMatrix();
		}
		
	private:
		/**
		  * @param height the height in the grid
		  * @param row the row in the grid
		  * @param column the column in the grid
		  * @return true if the location specified is within the space occupied by the BlockStructure
		  */
		bool isInBounds(size_type height, size_type row, size_type column) const { return 	height >= 0 && height < (this -> height) && row >= 0 && row < rows && column >= 0 && column < columns; }
};

#endif /*BLOCKSTRUCTURE_H_*/
