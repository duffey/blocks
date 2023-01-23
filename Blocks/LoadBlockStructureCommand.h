#ifndef LOADBLOCKSTRUCTURECOMMAND_H_
#define LOADBLOCKSTRUCTURECOMMAND_H_

#include <string>	//string

#include "AbstractCommand.h"
#include "BlockDriver.h"
#include "NxPhysics.h"

/**
  * @brief Given an instance of BlockDriver, a file path, and a few other arguments, this Command loads the BlockDriver with a new BlockStructure.
  */
class LoadBlockStructureCommand : public AbstractCommand
{
	private:
		BlockDriver& blockDriver;
		const string filePath;
		NxScene& physxScene;
		GLfloat blockSize;
		Vector4 base;
		
	public:
		LoadBlockStructureCommand(BlockDriver& blockDriver, const string& filePath, NxScene& physxScene, const GLfloat& blockSize, const Vector4& base = Vector4(0.0, 0.0, 0.0, 1.0)) : blockDriver(blockDriver), filePath(filePath), physxScene(physxScene), blockSize(blockSize), base(base) {}
		
		virtual void execute() const
		{
			blockDriver.reset();
			blockDriver.loadBlockStructure(new BlockStructure(filePath, physxScene, blockSize, base));
		}
};

#endif /*LOADBLOCKSTRUCTURECOMMAND_H_*/
