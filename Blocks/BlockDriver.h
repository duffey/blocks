#ifndef BLOCKDRIVER_H_
#define BLOCKDRIVER_H_

#include <vector>		//vector
#include <cassert>		//assert
#include <iostream>		//REMOVE
#include <cmath>		//abs
#include <memory>		//auto_ptr
#include <stdexcept>	//runtime_error
#include <time.h>

#include "BlockStructure.h"
#include "Matrix44.h"
#include "Vector4.h"
#include "DrawObjects.h"

using namespace std;

/**
  * @brief This class drives much of the interaction.
  */
class BlockDriver
{
	public:
		typedef BlockStructure::size_type size_type;

	private:
		enum { x, y, z, w };
		bool loaded;
		const GLfloat thresholdRatio;
		BlockStructure* blockStructure;
		
		/**
		  * This struct assists in organizing the space of the game.
		  */
		struct Voxel
		{
			GLint height, row, column;
			
			Voxel(GLint height, GLint row, GLint column) : height(height), row(row), column(column) {}
			
			/**
			  * @param other an instance of Voxel
			  * @return true if the two Voxels exists at the same height, row, or column
			  */
			bool isAlongSameAxis(const Voxel& other) const
			{
				return (height - other.height != 0) ^ (row - other.row != 0) ^ (column - other.column != 0);
			}
			
			//NOTE: Can I optimize this?
			/**
			  * @param left an instance of Voxel
			  * @param right an instance of Voxel
			  * @returns true if the Voxel is inbetween left and right in Taxicab Geometry
			  */
			bool isInBetween(const Voxel& left, const Voxel& right) const
			{
				return isAlongSameAxis(left) && isAlongSameAxis(right) && ((height >= left.height && row >= left.row && column >= left.column && height <= right.height && row <= right.row && column <= right.column)  || (height >= right.height && row >= right.row && column >= right.column && height <= left.height && row <= left.row && column <= left.column));
			}
			
			/**
			  * @return true if the Voxels are at the same height, row, and column
			  */
			friend bool operator == (const Voxel& lhs, const Voxel& rhs)
			{
				return (lhs.height == rhs.height) && (lhs.row == rhs.row) && (lhs.column == rhs.column);
			}
		};

	public:
		//NOTE: Need to figure out which functionality can be defined in the abstract class v.s. the implementation
		class Laser
		{
			private:
				virtual bool isAtPastLocation() const = 0;

				virtual bool hasTransitioned() const = 0;
				
				virtual bool isWithinTurnThreshold() const = 0;
				
				virtual bool hasPassedBlockCenter() const = 0;

			public:
				enum Direction {UP, DOWN, LEFT, RIGHT};
				
				virtual void turn(Direction turnDirection) = 0;
				
				virtual void move() = 0;
				
				virtual void setMobile(bool mobile) = 0;
				
				virtual void draw() const = 0;

				virtual bool isIdle() const = 0;
				
				virtual bool isMobile() const = 0;
		};
		
	private:
		class LaserImplementation : public Laser
		{
			private:
				static const GLfloat secondsToIdle;
				static const GLfloat laserWidth; 
				static const GLfloat moveInterval;

				bool mobile;
				BlockDriver& blockDriver;
				BlockStructure* blockStructure;
				GLfloat speed, turnThreshold, moveProgress;
				Vector4 currentDirection, nextDirection, currentLocation, currentVoxelLocation;
				Matrix44 currentOrientation, nextOrientation;
				Voxel currentVoxel;
				vector<Voxel > visitedLocations;
				clock_t  lastMoved;
				
			public:
				LaserImplementation(BlockDriver& blockDriver,
									const Vector4& currentDirection,
									const Voxel& currentVoxel) :
				  
									mobile(true),
									blockDriver(blockDriver),
									blockStructure(blockDriver.getBlockStructure()),
									speed(blockStructure -> getBlockSize() / 30.0),
									turnThreshold(blockStructure -> getBlockSize() * blockDriver.thresholdRatio / 2.0),
									moveProgress(0.0),
									currentDirection(currentDirection),
									nextDirection(currentDirection),
									currentOrientation(),
									nextOrientation(currentOrientation),
									currentVoxel(currentVoxel),
									lastMoved(clock())
				{
					visitedLocations.push_back(this -> currentVoxel);
					
					currentVoxelLocation = blockDriver.getVoxelLocation(this -> currentVoxel);
					
					currentLocation[x] = currentVoxelLocation[x] - this -> currentDirection[x] * blockStructure -> getBlockSize() / 2.0;
					currentLocation[y] = currentVoxelLocation[y] - this -> currentDirection[y] * blockStructure -> getBlockSize() / 2.0;
					currentLocation[z] = currentVoxelLocation[z] - this -> currentDirection[z] * blockStructure -> getBlockSize() / 2.0;
					currentLocation[w] = 1.0;
				}
				
				virtual void turn(Direction turnDirection)
				{
					if(isWithinTurnThreshold())
					{
						nextDirection = currentDirection;
						//nextOrientation = currentOrientation;

						//NOTE: Too expensive (We avoid needlessly copying currentOrientation.)
						nextDirection = currentOrientation * nextDirection;
						
						glPushMatrix();
							glLoadIdentity();
							glMultMatrixf(currentOrientation);
						
							switch(turnDirection)
							{
								case UP:	glRotatef(90.0, 0, 0, 1);
											break;
											
								case DOWN:	glRotatef(-90.0, 0, 0, 1);	
											break;
								
								case RIGHT:	glRotatef(-90.0, 0, 1, 0);
											break;
											
								case LEFT:	glRotatef(90.0, 0, 1, 0);
											break;
							}
						
							glGetFloatv(GL_MODELVIEW_MATRIX, nextOrientation);
						glPopMatrix();
			
						//NOTE: Too expensive
						//Change of basis back to the standard basis	
						nextDirection = nextOrientation.getTranspose() * nextDirection;
						
						//Fixes rounding errors
						for(Vector4::iterator i = nextDirection.begin(); i != nextDirection.end(); i++)
							*i = (GLint)(*i);
					}
				}
				
				virtual void setMobile(bool mobile) { this -> mobile = mobile; }
				
				virtual void move()
				{
					assert(blockDriver.isLoaded());

					currentVoxelLocation = blockDriver.getVoxelLocation(currentVoxel);
					GLfloat elapsedTime = (GLfloat)(clock() - lastMoved) / CLOCKS_PER_SEC;
					
					if(isMobile() && elapsedTime > moveInterval)
					{
						lastMoved = clock();
						moveProgress += speed;
						
						if(!hasPassedBlockCenter())
						{	
							currentLocation[x] = currentVoxelLocation[x] + (-blockStructure -> getBlockSize() / 2.0 + moveProgress) * currentDirection[x];
							currentLocation[y] = currentVoxelLocation[y] + (-blockStructure -> getBlockSize() / 2.0 + moveProgress) * currentDirection[y];
							currentLocation[z] = currentVoxelLocation[z] + (-blockStructure -> getBlockSize() / 2.0 + moveProgress) * currentDirection[z];
						}
						else
						{	
							currentLocation[x] = currentVoxelLocation[x] + (-blockStructure -> getBlockSize() / 2.0 + moveProgress) * nextDirection[x];
							currentLocation[y] = currentVoxelLocation[y] + (-blockStructure -> getBlockSize() / 2.0 + moveProgress) * nextDirection[y];
							currentLocation[z] = currentVoxelLocation[z] + (-blockStructure -> getBlockSize() / 2.0 + moveProgress) * nextDirection[z];
							
							if(hasTransitioned())
							{
								assert(hasPassedBlockCenter());
								
								moveProgress = 0.0;
								currentDirection = nextDirection;
								currentOrientation = nextOrientation;
								currentVoxel.height += (GLint)currentDirection[y];
								currentVoxel.row += (GLint)currentDirection[z];
								currentVoxel.column += (GLint)currentDirection[x];
								
								if(blockStructure -> hasBlock(currentVoxel.height, currentVoxel.row, currentVoxel.column))
								{
									blockStructure -> touchBlock(currentVoxel.height, currentVoxel.row, currentVoxel.column);
									
									//We lost.
									if(blockStructure -> hasImpenetrableBlock(currentVoxel.height, currentVoxel.row, currentVoxel.column))
									{
										setMobile(false);
										blockDriver.setAllLossState();
									}
									//We won!
									else if(blockDriver.hasTouchedAllPenetrable())
									{
										setMobile(false);
										blockDriver.setAllVictoryState();
									}
								}
								
								//NOTE: May need to comment out this optimization to visualize bugs involving the lazer path
								//No need to store the midpoint along a line segment where the points change with respect to one axis.
								if(visitedLocations.size() > 1 && currentVoxel.isAlongSameAxis(visitedLocations[visitedLocations.size() - 2]))
									visitedLocations.pop_back();
									
								visitedLocations.push_back(currentVoxel);
							}
						}
						
						if(isAtPastLocation())
						{
							//Advance the moveProgress for aesthetic purposes (to visualize the collision)
							moveProgress = blockStructure -> getBlockSize();
							
							currentLocation[x] = currentVoxelLocation[x] + (-blockStructure -> getBlockSize() / 2.0 + moveProgress) * currentDirection[x];
							currentLocation[y] = currentVoxelLocation[y] + (-blockStructure -> getBlockSize() / 2.0 + moveProgress) * currentDirection[y];
							currentLocation[z] = currentVoxelLocation[z] + (-blockStructure -> getBlockSize() / 2.0 + moveProgress) * currentDirection[z];
							
							setMobile(false);
							blockDriver.setAllLossState();
						}
					}
				}
				
				virtual void draw() const
				{
					assert(visitedLocations.back() == currentVoxel);
					
					glMatrixMode(GL_MODELVIEW);
					
					if(isMobile())									glColor3f(1.0, 0.5, 0.0);
					else if(blockDriver.hasTouchedAllPenetrable())	glColor3f(0.0, 1.0, 0.0);
					else											glColor3f(1.0, 0.0, 0.0);
						
					glEnable(GL_LINE_SMOOTH);
					glLineWidth(laserWidth);
					glBegin(GL_LINE_STRIP);
					
						for(vector<Voxel>::const_iterator i = visitedLocations.begin(); i != visitedLocations.end() - 1; i++)
							glVertex3fv(blockDriver.getVoxelLocation(*i).data());
						
						//Don't draw the line to the current voxel's center until we have passed this location.
						if(hasPassedBlockCenter())
							glVertex3fv(blockDriver.getVoxelLocation(*(visitedLocations.end() - 1)).data());
						
						glVertex3fv(currentLocation.data());
						
					glEnd();
					glDisable(GL_LINE_SMOOTH);
				}
				
				virtual bool isIdle() const { return (GLfloat)(clock() - lastMoved) / CLOCKS_PER_SEC >= secondsToIdle; }

				virtual bool isMobile() const { return mobile; }
				
			private:
				virtual bool isAtPastLocation() const
				{
					for(vector<Voxel>::const_iterator i = visitedLocations.begin(); i != visitedLocations.end() - 1; i++)
						if(currentVoxel == *i || currentVoxel.isInBetween(*i, *(i + 1)))
						{
							Vector4 voxel1 = blockDriver.getVoxelLocation(*i);
							Vector4 voxel2 = blockDriver.getVoxelLocation(*(i + 1));
							
							return true;
						}
					
					return false;
				}

				virtual bool hasTransitioned() const
				{
					//NOTE: Consider these if necessary
					//NOTE: This takes the 1-norm (Manhattan distance) distance which only works in this special case.  Consider the 2-norm if necessary.
					//return (abs(currentLocation[x] - currentVoxelLocation[x]) + abs(currentLocation[y] - currentVoxelLocation[y]) + abs(currentLocation[z] - currentVoxelLocation[z])) > blockStructure -> getBlockSize() / 2.0;
					//return sqrt(pow(currentLocation[x] - currentVoxelLocation[x], 2) + pow(currentLocation[y] - currentVoxelLocation[y], 2) + pow(currentLocation[z] - currentVoxelLocation[z], 2)) > blockStructure -> getBlockSize() / 2.0;
					return moveProgress > (blockStructure -> getBlockSize());
				}
				
				virtual bool isWithinTurnThreshold() const
				{
					Vector4 currentVoxelLocation = blockDriver.getVoxelLocation(currentVoxel);
					
					//NOTE: Consider these if necessary
					//NOTE: This takes the 1-norm (Manhattan distance) distance which only works in this special case.  Consider the 2-norm distance if necessary.
					//return (abs(currentLocation[x] - currentVoxelLocation[x]) + abs(currentLocation[y] - currentVoxelLocation[y]) + abs(currentLocation[z] - currentVoxelLocation[z])) < turnThreshold;
					//return sqrt(pow(currentLocation[x] - currentVoxelLocation[x], 2) + pow(currentLocation[y] - currentVoxelLocation[y], 2) + pow(currentLocation[z] - currentVoxelLocation[z], 2)) < turnThreshold;
					return moveProgress >= blockStructure -> getBlockSize() / 2.0 - turnThreshold && moveProgress <= blockStructure -> getBlockSize() / 2.0 + turnThreshold;
				}
				
				virtual bool hasPassedBlockCenter() const
				{
					return moveProgress > blockStructure -> getBlockSize() / 2.0;
				}
		};
		
	public:
		BlockDriver(BlockStructure* blockStructure = NULL) : thresholdRatio(1.0), blockStructure(blockStructure) { loaded = (blockStructure != NULL); }
		
		~BlockDriver()
		{
			reset();
		}
		/**
		  * Attaches an instance of BlockStructue to the BlockDriver.  This method affects the nature of Lasers returned by getLaser().
		  * @see getLaser
		  * @param blockStructure an instance of BlockStructure
		  */
		void loadBlockStructure(BlockStructure* blockStructure)
		{
			this -> blockStructure = blockStructure;
			loaded = true;
		}

		void unload() { loaded = false; }

		/**
		  * deletes any existing BlockStructure associated with the BlockDriver
		  */
		void reset() { if(blockStructure != NULL) delete blockStructure; }
		
		/**
		  * @return an auto_ptr to an instance of Laser, which can be controlled by the player
		  */
		auto_ptr<Laser> getLaser()
		{
			assert(isLoaded());

			return auto_ptr<Laser>(new LaserImplementation(*this, Vector4(1.0, 0.0, 0.0, 1.0), Voxel(0, 0, -5)));
		}
		
		/**
		  * This method, which can be used for debugging purposes, draws a grid around the BlockStructure associated with the BlockDriver.
		  * It will throw a runtime_error if no BlockStructure has been loaded.
		  */
		void drawVoxelGrid() const
		{
			if(!isLoaded())
				throw runtime_error("There is no BlockStructure associated with this BlockDriver.");

			assert(isLoaded());

			const Vector4& base = blockStructure -> getBase();
			const GLfloat& blockSize = blockStructure -> getBlockSize();
			const size_type& height = blockStructure -> getHeight();
			const size_type& rows = blockStructure -> getRows();
			const size_type& columns = blockStructure -> getColumns();
			
			//NOTE: Need to stop computing this so many times
			GLfloat upperLeftCornerX = base[x] + (columns / 2) * -blockSize, upperLeftCornerZ = base[z] + (rows / 2) * -blockSize;

			//This computes the absolute upper left hand corner of the base of the structure.
			if(columns % 2 == 0)
				upperLeftCornerX += blockSize / 2.0;

			if(rows % 2 == 0)
				upperLeftCornerZ += blockSize / 2.0;
			
			glColor3f(1.0, 1.0, 1.0);
			glPushMatrix();	
				
				for(size_type i = 0; i < height; i++)
					for(size_type j = 0; j < rows; j++)
						for(size_type k = 0; k < columns; k++)
						{
							glPushMatrix();
								glTranslatef(upperLeftCornerX + k * blockSize, base[y] + blockSize / 2.0 + blockSize * i, upperLeftCornerZ + j * blockSize);
								glutWireCube(blockSize);
							glPopMatrix();
						}
						
			glPopMatrix();
		}
		
		/**
		  * @param height The height of the Voxel
		  * @param row The row of the Voxel
		  * @param column The column of the Voxel
		  * @return a positional vector describing the location of the Voxel
		  */
		const Vector4 getVoxelLocation(GLint height, GLint row, GLint column) const
		{
			assert(isLoaded());
			
			const Vector4& base = blockStructure -> getBase();
			const GLfloat& blockSize = blockStructure -> getBlockSize();
			const size_type& rows = blockStructure -> getRows();
			const size_type& columns = blockStructure -> getColumns();
			
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
		  * @param an instance of Voxel
		  * @return a positional vector describing the location of the Voxel
		  */
		const Vector4 getVoxelLocation(const Voxel& voxel) const
		{
			assert(isLoaded());
			
			const Vector4& base = blockStructure -> getBase();
			const GLfloat& blockSize = blockStructure -> getBlockSize();
			const size_type& rows = blockStructure -> getRows();
			const size_type& columns = blockStructure -> getColumns();
			
			//NOTE: Need to stop computing this so many times
			GLfloat upperLeftCornerX = base[x] - (columns / 2) * blockSize, upperLeftCornerZ = base[z] - (rows / 2) * blockSize;

			if(columns % 2 == 0)
				upperLeftCornerX += blockSize / 2.0;

			if(rows % 2 == 0)
				upperLeftCornerZ += blockSize / 2.0;
			
			return Vector4(	upperLeftCornerX + voxel.column * blockSize,
							base[y] + blockSize / 2.0 + blockSize * voxel.height,
							upperLeftCornerZ + voxel.row * blockSize,
							1.0);
		}

		bool hasTouchedAllPenetrable() const
		{
			assert(isLoaded());

			const size_type& height = blockStructure -> getHeight();
			const size_type& rows = blockStructure -> getRows();
			const size_type& columns = blockStructure -> getColumns();

			for(size_type i = 0; i < height; i++)
				for(size_type j = 0; j < rows; j++)
					for(size_type k = 0; k < columns; k++)
						if(blockStructure -> hasBlock(i, j, k) && !blockStructure -> hasImpenetrableBlock(i, j, k) && !blockStructure -> hasTouchedBlock(i, j, k))
							return false;

			return true;
		}

		void setAllLossState()
		{
			assert(isLoaded());

			const size_type& height = blockStructure -> getHeight();
			const size_type& rows = blockStructure -> getRows();
			const size_type& columns = blockStructure -> getColumns();

			for(size_type i = 0; i < height; i++)
				for(size_type j = 0; j < rows; j++)
					for(size_type k = 0; k < columns; k++)
						if(blockStructure -> hasBlock(i, j, k))
							blockStructure -> setBlockLossState(i, j, k);
		}

		void setAllVictoryState()
		{
			assert(isLoaded());

			const size_type& height = blockStructure -> getHeight();
			const size_type& rows = blockStructure -> getRows();
			const size_type& columns = blockStructure -> getColumns();

			for(size_type i = 0; i < height; i++)
				for(size_type j = 0; j < rows; j++)
					for(size_type k = 0; k < columns; k++)
						if(blockStructure -> hasBlock(i, j, k))
							blockStructure -> setBlockVictoryState(i, j, k);
		}
		
		/**
		  * This method, which can be used for debugging purposes, highlights a Voxel existing in the space defined by the BlockStructure grid.
		  * @param height The height of the Voxel
		  * @param row The row of the Voxel
		  * @param column The column of the Voxel
		  */
		void highlightVoxel(GLint height, GLint row, GLint column) const
		{
			const GLfloat& blockSize = blockStructure -> getBlockSize();
			Vector4 voxelLocation = getVoxelLocation(height, row, column);
				
			glPushMatrix();
				glTranslatef(voxelLocation[x], voxelLocation[y], voxelLocation[z]);
				glutSolidCube(blockSize);
			glPopMatrix();
		}
		
		//NOTE: Reconsider placement of this method.
		/**
		  * This method, which can be used for debugging purposes, draws the area in which an instance of Laser can successfully turn.
		  * @see Laser
		  * @see Laser::turn
		  */
		void showVoxelTurnThreshold(GLint height, GLint row, GLint column) const
		{
			const GLfloat& blockSize = blockStructure -> getBlockSize();
			Vector4 voxelLocation = getVoxelLocation(height, row, column);
				
			glPushMatrix();
				glTranslatef(voxelLocation[x], voxelLocation[y], voxelLocation[z]);
				glutSolidCube(blockSize * thresholdRatio);
			glPopMatrix();
		}
		
		/**
		  * @return true if the BlockDriver has an instance of BlockStructure associated with it
		  */
		bool isLoaded() const { return loaded; }
		
		//NOTE: This method is used by Laser and should be removed.
		//Instead, new methods should be added to the interface of BlockDriver for manipulating the BlockStructure.
		/**
		  * @return a pointer to the associated BlockStructure
		  */
		BlockStructure* getBlockStructure() { return blockStructure; }
};

const GLfloat BlockDriver::LaserImplementation::secondsToIdle = 5.0;
const GLfloat BlockDriver::LaserImplementation::laserWidth = 5.0;
const GLfloat BlockDriver::LaserImplementation::moveInterval = 0.01;

#endif /*BLOCKDRIVER_H_*/
 